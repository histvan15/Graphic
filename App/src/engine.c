#include "engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


vec3 vec3_add(vec3 a, vec3 b) { return (vec3){a.x + b.x, a.y + b.y, a.z + b.z}; }
vec3 vec3_sub(vec3 a, vec3 b) { return (vec3){a.x - b.x, a.y - b.y, a.z - b.z}; }
vec3 vec3_scale(vec3 v, float s) { return (vec3){v.x * s, v.y * s, v.z * s}; }
vec3 vec3_cross(vec3 a, vec3 b) { 
    return (vec3){a.y*b.z - a.z*b.y,  a.z*b.x - a.x*b.z,  a.x*b.y - a.y*b.x}; 
}
float vec3_dot(vec3 a, vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
vec3 vec3_normalize(vec3 v) { 
    float len = sqrt(vec3_dot(v, v)); 
    return len == 0 ? v : vec3_scale(v, 1.0f/len); 
}
float radians(float deg) { return deg * (M_PI / 180.0f); }

void mat4_identity(float m[16]) {
    for(int i=0; i<16; i++) m[i] = 0.0f;
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

void mat4_perspective(float fovy, float aspect, float near_z, float far_z, float m[16]) {
    for(int i=0; i<16; i++) m[i] = 0.0f;
    float tanHalfFovy = tan(fovy / 2.0f);
    m[0] = 1.0f / (aspect * tanHalfFovy);
    m[5] = 1.0f / tanHalfFovy;
    m[10] = -(far_z + near_z) / (far_z - near_z);
    m[11] = -1.0f;
    m[14] = -(2.0f * far_z * near_z) / (far_z - near_z);
}

void mat4_lookAt(vec3 eye, vec3 center, vec3 up, float m[16]) {
    vec3 f = vec3_normalize(vec3_sub(center, eye));
    vec3 s = vec3_normalize(vec3_cross(f, up));
    vec3 u = vec3_cross(s, f);
    mat4_identity(m);
    m[0] = s.x;  m[4] = s.y;  m[8]  = s.z;
    m[1] = u.x;  m[5] = u.y;  m[9]  = u.z;
    m[2] = -f.x; m[6] = -f.y; m[10] = -f.z;
    m[12] = -vec3_dot(s, eye);
    m[13] = -vec3_dot(u, eye);
    m[14] =  vec3_dot(f, eye);
}

char* loadShaderSource(const char* filePath) {
    FILE* file = fopen(filePath, "rb");
    if (!file) {
        printf("Hiba: Nem talalhato a shader fajl: %s\n", filePath);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* buffer = (char*)malloc(length + 1);
    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    
    fclose(file);
    return buffer;
}

unsigned int createProgram(const char* vertexPath, const char* fragmentPath) {
    char* vSrc = loadShaderSource(vertexPath);
    char* fSrc = loadShaderSource(fragmentPath);
    if (!vSrc || !fSrc) return 0;

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const char**)&vSrc, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const char**)&fSrc, NULL);
    glCompileShader(fragmentShader);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    free(vSrc);
    free(fSrc);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

float* loadOBJ(const char* path, int* out_vertexCount) {
    FILE* file = fopen(path, "r");
    if (!file) {
        printf("Hiba: Nem talalhato a modell fajl: %s\n", path);
        *out_vertexCount = 0;
        return NULL;
    }
    vec3* temp_vertices = (vec3*)malloc(10000 * sizeof(vec3));
    vec2* temp_uvs = (vec2*)malloc(10000 * sizeof(vec2));
    vec3* temp_normals = (vec3*)malloc(10000 * sizeof(vec3));
    int v_cnt = 0, vt_cnt = 0, vn_cnt = 0;
    int max_out = 30000;
    float* out_data = (float*)malloc(max_out * 8 * sizeof(float));
    int out_count = 0;
    char lineHeader[128];
    while (fscanf(file, "%127s", lineHeader) != EOF) {
        if (strcmp(lineHeader, "v") == 0) {
            fscanf(file, "%f %f %f\n", &temp_vertices[v_cnt].x, &temp_vertices[v_cnt].y, &temp_vertices[v_cnt].z); v_cnt++;
        } else if (strcmp(lineHeader, "vt") == 0) {
            fscanf(file, "%f %f\n", &temp_uvs[vt_cnt].x, &temp_uvs[vt_cnt].y); vt_cnt++;
        } else if (strcmp(lineHeader, "vn") == 0) {
            fscanf(file, "%f %f %f\n", &temp_normals[vn_cnt].x, &temp_normals[vn_cnt].y, &temp_normals[vn_cnt].z); vn_cnt++;
        } else if (strcmp(lineHeader, "f") == 0) {
            int v[3], vt[3], vn[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &v[0], &vt[0], &vn[0], &v[1], &vt[1], &vn[1], &v[2], &vt[2], &vn[2]);
            if (matches != 9) { printf("Hiba: %s formatuma nem jo!\n", path); break; }
            for (int i = 0; i < 3; i++) {
                out_data[out_count*8 + 0] = temp_vertices[v[i]-1].x;
                out_data[out_count*8 + 1] = temp_vertices[v[i]-1].y;
                out_data[out_count*8 + 2] = temp_vertices[v[i]-1].z;
                out_data[out_count*8 + 3] = temp_uvs[vt[i]-1].x;
                out_data[out_count*8 + 4] = temp_uvs[vt[i]-1].y;
                out_data[out_count*8 + 5] = temp_normals[vn[i]-1].x;
                out_data[out_count*8 + 6] = temp_normals[vn[i]-1].y;
                out_data[out_count*8 + 7] = temp_normals[vn[i]-1].z;
                out_count++;
            }
        }
    }
    *out_vertexCount = out_count;
    free(temp_vertices); free(temp_uvs); free(temp_normals);
    fclose(file);
    return out_data;
}

unsigned int loadCubemap(char* faces[]) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    int width, height, nrChannels;
    for (unsigned int i = 0; i < 6; i++) {
        unsigned char *data = stbi_load(faces[i], &width, &height, &nrChannels, 3);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else { printf("Hiba a Skybox textura betoltesekor: %s\n", faces[i]); }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return textureID;
}

unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        printf("Hiba: Nem sikerult betolteni a texturat: %s\n", path);
    }
    stbi_image_free(data);
    return textureID;
}

void setupMesh(float* vertices, size_t size, unsigned int* vao, unsigned int* vbo) {
    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    
    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void drawMesh(unsigned int vao, unsigned int texture, int vertexCount, int modelLoc, float* modelMatrix) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMatrix);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}