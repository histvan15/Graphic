#define SDL_MAIN_HANDLED
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct { float x, y; } vec2;
typedef struct { float x, y, z; } vec3;

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
    int out_cnt = 0;
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
                out_data[out_cnt*8 + 0] = temp_vertices[v[i]-1].x;
                out_data[out_cnt*8 + 1] = temp_vertices[v[i]-1].y;
                out_data[out_cnt*8 + 2] = temp_vertices[v[i]-1].z;
                out_data[out_cnt*8 + 3] = temp_uvs[vt[i]-1].x;
                out_data[out_cnt*8 + 4] = temp_uvs[vt[i]-1].y;
                out_data[out_cnt*8 + 5] = temp_normals[vn[i]-1].x;
                out_data[out_cnt*8 + 6] = temp_normals[vn[i]-1].y;
                out_data[out_cnt*8 + 7] = temp_normals[vn[i]-1].z;
                out_cnt++;
            }
        }
    }
    *out_vertexCount = out_cnt;
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

vec3 cameraPos   = {0.0f, 1.0f,  4.0f};
vec3 cameraFront = {0.0f, 0.0f, -1.0f};
vec3 cameraUp    = {0.0f, 1.0f,  0.0f};
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float yaw   = -90.0f;
float pitch = -10.0f; 
float lightIntensity = 1.0f;

int main(int argc, char* argv[]) {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow("Sziget Projekt", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) return 1;

    SDL_SetRelativeMouseMode(SDL_TRUE); 
    glEnable(GL_DEPTH_TEST);
    
    // blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int shaderProgram = createProgram("shaders/main.vert", "shaders/main.frag");
    unsigned int waterProgram  = createProgram("shaders/water.vert", "shaders/water.frag");
    unsigned int skyboxProgram = createProgram("shaders/skybox.vert", "shaders/skybox.frag");

    // obj 
    int objVertexCount = 0;
    float* objVertices = loadOBJ("models/piramid.obj", &objVertexCount);
    unsigned int objVBO, objVAO;
    if (objVertices != NULL) {
        glGenVertexArrays(1, &objVAO);
        glGenBuffers(1, &objVBO);
        glBindVertexArray(objVAO);
        glBindBuffer(GL_ARRAY_BUFFER, objVBO);
        glBufferData(GL_ARRAY_BUFFER, objVertexCount * 8 * sizeof(float), objVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); glEnableVertexAttribArray(2);
        free(objVertices);
    }

    // ground
    float groundVertices[] = {
        -5.0f, 0.0f, -5.0f,    0.0f, 5.0f,    0.0f, 1.0f, 0.0f, 
         5.0f, 0.0f, -5.0f,    5.0f, 5.0f,    0.0f, 1.0f, 0.0f,
         5.0f, 0.0f,  5.0f,    5.0f, 0.0f,    0.0f, 1.0f, 0.0f,
         5.0f, 0.0f,  5.0f,    5.0f, 0.0f,    0.0f, 1.0f, 0.0f,
        -5.0f, 0.0f,  5.0f,    0.0f, 0.0f,    0.0f, 1.0f, 0.0f,
        -5.0f, 0.0f, -5.0f,    0.0f, 5.0f,    0.0f, 1.0f, 0.0f
    };
    unsigned int groundVBO, groundVAO;
    glGenVertexArrays(1, &groundVAO); glGenBuffers(1, &groundVBO);
    glBindVertexArray(groundVAO); glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); glEnableVertexAttribArray(2);

    // water
    float waterVertices[] = {
        -50.0f, -0.05f, -50.0f,   0.0f, 25.0f,   0.0f, 1.0f, 0.0f, 
         50.0f, -0.05f, -50.0f,  25.0f, 25.0f,   0.0f, 1.0f, 0.0f,
         50.0f, -0.05f,  50.0f,  25.0f,  0.0f,   0.0f, 1.0f, 0.0f,
         50.0f, -0.05f,  50.0f,  25.0f,  0.0f,   0.0f, 1.0f, 0.0f,
        -50.0f, -0.05f,  50.0f,   0.0f,  0.0f,   0.0f, 1.0f, 0.0f,
        -50.0f, -0.05f, -50.0f,   0.0f, 25.0f,   0.0f, 1.0f, 0.0f
    };
    unsigned int waterVBO, waterVAO;
    glGenVertexArrays(1, &waterVAO); glGenBuffers(1, &waterVBO);
    glBindVertexArray(waterVAO); glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertices), waterVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); glEnableVertexAttribArray(2);

    // skybox
    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
    };
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO); glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO); glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    stbi_set_flip_vertically_on_load(false); 
    char* faces[] = {
        "textures/skybox/right.png", "textures/skybox/left.png",
        "textures/skybox/top.png", "textures/skybox/bottom.png",
        "textures/skybox/front.png", "textures/skybox/back.png"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned int texPyramid, texGround, texWater;

    glGenTextures(1, &texPyramid); glBindTexture(GL_TEXTURE_2D, texPyramid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unsigned char *data1 = stbi_load("textures/piramid.jpg", &width, &height, &nrChannels, 0);
    if (data1) { glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1); glGenerateMipmap(GL_TEXTURE_2D); } stbi_image_free(data1); 
    
    glGenTextures(1, &texGround); glBindTexture(GL_TEXTURE_2D, texGround);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unsigned char *data2 = stbi_load("textures/ground.jpg", &width, &height, &nrChannels, 0);
    if (data2) { glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2); glGenerateMipmap(GL_TEXTURE_2D); } stbi_image_free(data2);

    glGenTextures(1, &texWater); glBindTexture(GL_TEXTURE_2D, texWater);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unsigned char *data3 = stbi_load("textures/water.jpg", &width, &height, &nrChannels, 0);
    if (data3) { glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3); glGenerateMipmap(GL_TEXTURE_2D); } stbi_image_free(data3);

    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc  = glGetUniformLocation(shaderProgram, "view");
    int projLoc  = glGetUniformLocation(shaderProgram, "projection");
    int lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");
    int lightIntLoc = glGetUniformLocation(shaderProgram, "lightIntensity");

    int wModelLoc = glGetUniformLocation(waterProgram, "model");
    int wViewLoc  = glGetUniformLocation(waterProgram, "view");
    int wProjLoc  = glGetUniformLocation(waterProgram, "projection");
    int wLightDirLoc = glGetUniformLocation(waterProgram, "lightDir");
    int wLightIntLoc = glGetUniformLocation(waterProgram, "lightIntensity");
    int wTimeLoc = glGetUniformLocation(waterProgram, "time");

    int sbViewLoc = glGetUniformLocation(skyboxProgram, "view");
    int sbProjLoc = glGetUniformLocation(skyboxProgram, "projection");

    glUseProgram(skyboxProgram);
    glUniform1i(glGetUniformLocation(skyboxProgram, "skybox"), 0);

    int running = 1;
    SDL_Event event;
    while (running) {
        float currentFrame = SDL_GetTicks() / 1000.0f;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_MOUSEMOTION) {
                float xoffset = event.motion.xrel * 0.1f;
                float yoffset = -event.motion.yrel * 0.1f;
                yaw   += xoffset;
                pitch += yoffset;
                if (pitch > 89.0f)  pitch = 89.0f;
                if (pitch < -89.0f) pitch = -89.0f;
                vec3 front;
                front.x = cos(radians(yaw)) * cos(radians(pitch));
                front.y = sin(radians(pitch));
                front.z = sin(radians(yaw)) * cos(radians(pitch));
                cameraFront = vec3_normalize(front);
            }
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);
        float cameraSpeed = 2.5f * deltaTime;

        // movement
        vec3 nextPos = cameraPos;
        
        if (state[SDL_SCANCODE_W]) nextPos = vec3_add(nextPos, vec3_scale(cameraFront, cameraSpeed));
        if (state[SDL_SCANCODE_S]) nextPos = vec3_sub(nextPos, vec3_scale(cameraFront, cameraSpeed));
        if (state[SDL_SCANCODE_A]) nextPos = vec3_sub(nextPos, vec3_scale(vec3_normalize(vec3_cross(cameraFront, cameraUp)), cameraSpeed));
        if (state[SDL_SCANCODE_D]) nextPos = vec3_add(nextPos, vec3_scale(vec3_normalize(vec3_cross(cameraFront, cameraUp)), cameraSpeed));

        if (state[SDL_SCANCODE_ESCAPE]) running = 0;

        float minX = -0.6f; float maxX = 0.6f;
        float minZ = -0.6f; float maxZ = 0.6f;

        bool collisionX = (nextPos.x > minX && nextPos.x < maxX && cameraPos.z > minZ && cameraPos.z < maxZ);
        bool collisionZ = (cameraPos.x > minX && cameraPos.x < maxX && nextPos.z > minZ && nextPos.z < maxZ);

        if (!collisionX) cameraPos.x = nextPos.x;
        if (!collisionZ) cameraPos.z = nextPos.z;
        cameraPos.y = nextPos.y;

        // lights
        if (state[SDL_SCANCODE_KP_PLUS]  || state[SDL_SCANCODE_EQUALS]) lightIntensity += 1.0f * deltaTime;
        if (state[SDL_SCANCODE_KP_MINUS] || state[SDL_SCANCODE_MINUS])  lightIntensity -= 1.0f * deltaTime;
        if (lightIntensity > 1.5f) lightIntensity = 1.5f;
        if (lightIntensity < 0.0f) lightIntensity = 0.0f;

        glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        float projection[16];
        mat4_perspective(radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f, projection);
        float view[16];
        mat4_lookAt(cameraPos, vec3_add(cameraPos, cameraFront), cameraUp, view);

        // ground + obj
        glUseProgram(shaderProgram);
        glUniform3f(lightDirLoc, -0.5f, -1.0f, -0.3f); 
        glUniform1f(lightIntLoc, lightIntensity);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);

        glBindTexture(GL_TEXTURE_2D, texGround);
        glBindVertexArray(groundVAO);
        float modelGround[16];
        mat4_identity(modelGround);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelGround);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        if (objVertexCount > 0) {
            glBindTexture(GL_TEXTURE_2D, texPyramid);
            glBindVertexArray(objVAO);
            float modelPyramid[16];
            mat4_identity(modelPyramid);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelPyramid);
            glDrawArrays(GL_TRIANGLES, 0, objVertexCount);
        }

        // water
        glUseProgram(waterProgram);
        glUniform3f(wLightDirLoc, -0.5f, -1.0f, -0.3f); 
        glUniform1f(wLightIntLoc, lightIntensity);
        glUniformMatrix4fv(wProjLoc, 1, GL_FALSE, projection);
        glUniformMatrix4fv(wViewLoc, 1, GL_FALSE, view);
        glUniform1f(wTimeLoc, currentFrame); 

        glBindTexture(GL_TEXTURE_2D, texWater);
        glBindVertexArray(waterVAO);
        float modelWater[16];
        mat4_identity(modelWater);
        glUniformMatrix4fv(wModelLoc, 1, GL_FALSE, modelWater);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // skybox
        glDepthFunc(GL_LEQUAL); 
        glUseProgram(skyboxProgram);
        glUniformMatrix4fv(sbViewLoc, 1, GL_FALSE, view);
        glUniformMatrix4fv(sbProjLoc, 1, GL_FALSE, projection);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}