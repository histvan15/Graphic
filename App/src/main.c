#define SDL_MAIN_HANDLED
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


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

void mat4_rotateY(float m[16], float angle) {
    mat4_identity(m);
    float c = cos(angle);
    float s = sin(angle);
    m[0] = c;  m[8] = s;
    m[2] = -s; m[10] = c;
}

const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n" 
    "layout (location = 2) in vec3 aNormal;\n"
    "out vec2 TexCoord;\n" 
    "out vec3 Normal;\n" 
    "out vec3 FragPos;\n" 
    "out float Visibility;\n" 
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   vec4 worldPos = model * vec4(aPos, 1.0);\n"
    "   FragPos = vec3(worldPos);\n"
    "   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
    "   vec4 posRelativeToCam = view * worldPos;\n"
    "   gl_Position = projection * posRelativeToCam;\n"
    "   TexCoord = aTexCoord;\n"
    "   float distance = length(posRelativeToCam.xyz);\n"
    "   Visibility = exp(-pow((distance * 0.02), 2.0));\n" 
    "   Visibility = clamp(Visibility, 0.0, 1.0);\n"
    "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "in vec3 Normal;\n"
    "in vec3 FragPos;\n"
    "in float Visibility;\n"
    "uniform sampler2D texture1;\n"
    "uniform vec3 lightDir;\n"
    "uniform float lightIntensity;\n"
    "void main()\n"
    "{\n"
    "   vec4 texColor = texture(texture1, TexCoord);\n"
    "   float ambientStrength = 0.2;\n"
    "   vec3 ambient = ambientStrength * vec3(1.0);\n"
    "   vec3 norm = normalize(Normal);\n"
    "   vec3 lightDirNorm = normalize(-lightDir);\n"
    "   float diff = max(dot(norm, lightDirNorm), 0.0);\n"
    "   vec3 diffuse = diff * vec3(1.0) * lightIntensity;\n"
    "   vec3 result = (ambient + diffuse) * texColor.rgb;\n"
    "   vec3 fogColor = vec3(0.5f, 0.6f, 0.7f);\n" 
    "   FragColor = vec4(mix(fogColor, result, Visibility), texColor.a);\n" 
    "}\n\0";

// kamera
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

    SDL_Window* window = SDL_CreateWindow("Sziget Projekt - Tiszta C", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) return 1;

    SDL_SetRelativeMouseMode(SDL_TRUE); 
    glEnable(GL_DEPTH_TEST);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // piramis
    float pyramidVertices[] = { 
        -0.5f, 0.0f, -0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f, 
         0.5f, 0.0f, -0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  
         0.5f, 0.0f,  0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f, 
         0.5f, 0.0f,  0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,  
        -0.5f, 0.0f,  0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,  
        -0.5f, 0.0f, -0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f, 
        
        -0.5f, 0.0f,  0.5f,  0.0f, 0.0f,  0.0f, 0.447f, 0.894f,  
         0.5f, 0.0f,  0.5f,  1.0f, 0.0f,  0.0f, 0.447f, 0.894f,  
         0.0f, 1.0f,  0.0f,  0.5f, 1.0f,  0.0f, 0.447f, 0.894f,  
        
         0.5f, 0.0f, -0.5f,  0.0f, 0.0f,  0.0f, 0.447f, -0.894f,
        -0.5f, 0.0f, -0.5f,  1.0f, 0.0f,  0.0f, 0.447f, -0.894f,
         0.0f, 1.0f,  0.0f,  0.5f, 1.0f,  0.0f, 0.447f, -0.894f,
        
        -0.5f, 0.0f, -0.5f,  0.0f, 0.0f, -0.894f, 0.447f, 0.0f,
        -0.5f, 0.0f,  0.5f,  1.0f, 0.0f, -0.894f, 0.447f, 0.0f,
         0.0f, 1.0f,  0.0f,  0.5f, 1.0f, -0.894f, 0.447f, 0.0f,
        
         0.5f, 0.0f,  0.5f,  0.0f, 0.0f,  0.894f, 0.447f, 0.0f,
         0.5f, 0.0f, -0.5f,  1.0f, 0.0f,  0.894f, 0.447f, 0.0f, 
         0.0f, 1.0f,  0.0f,  0.5f, 1.0f,  0.894f, 0.447f, 0.0f 
    };
    unsigned int pyramidVBO, pyramidVAO;
    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);
    glBindVertexArray(pyramidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // talaj
    float groundVertices[] = {
        -20.0f, 0.0f, -20.0f,    0.0f, 10.0f,    0.0f, 1.0f, 0.0f, 
         20.0f, 0.0f, -20.0f,   10.0f, 10.0f,    0.0f, 1.0f, 0.0f,
         20.0f, 0.0f,  20.0f,   10.0f,  0.0f,    0.0f, 1.0f, 0.0f,
         20.0f, 0.0f,  20.0f,   10.0f,  0.0f,    0.0f, 1.0f, 0.0f,
        -20.0f, 0.0f,  20.0f,    0.0f,  0.0f,    0.0f, 1.0f, 0.0f,
        -20.0f, 0.0f, -20.0f,    0.0f, 10.0f,    0.0f, 1.0f, 0.0f
    };
    unsigned int groundVBO, groundVAO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // textúrák
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned int texPyramid, texGround;

    glGenTextures(1, &texPyramid);
    glBindTexture(GL_TEXTURE_2D, texPyramid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unsigned char *data1 = stbi_load("textures/piramid.jpg", &width, &height, &nrChannels, 0);
    if (data1) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else { printf("Hiba a piramis textura betoltesekor!\n"); }
    stbi_image_free(data1); 
    
    glGenTextures(1, &texGround);
    glBindTexture(GL_TEXTURE_2D, texGround);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unsigned char *data2 = stbi_load("textures/ground.jpg", &width, &height, &nrChannels, 0);
    if (data2) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else { printf("Hiba a talaj textura betoltesekor!\n"); }
    stbi_image_free(data2);

    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc  = glGetUniformLocation(shaderProgram, "view");
    int projLoc  = glGetUniformLocation(shaderProgram, "projection");
    int lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");
    int lightIntLoc = glGetUniformLocation(shaderProgram, "lightIntensity");


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
        
        // mozgás
        if (state[SDL_SCANCODE_W]) cameraPos = vec3_add(cameraPos, vec3_scale(cameraFront, cameraSpeed));
        if (state[SDL_SCANCODE_S]) cameraPos = vec3_sub(cameraPos, vec3_scale(cameraFront, cameraSpeed));
        if (state[SDL_SCANCODE_A]) cameraPos = vec3_sub(cameraPos, vec3_scale(vec3_normalize(vec3_cross(cameraFront, cameraUp)), cameraSpeed));
        if (state[SDL_SCANCODE_D]) cameraPos = vec3_add(cameraPos, vec3_scale(vec3_normalize(vec3_cross(cameraFront, cameraUp)), cameraSpeed));
        
        if (state[SDL_SCANCODE_ESCAPE]) running = 0; 
        
        if (state[SDL_SCANCODE_KP_PLUS]  || state[SDL_SCANCODE_EQUALS]) lightIntensity += 1.0f * deltaTime;
        if (state[SDL_SCANCODE_KP_MINUS] || state[SDL_SCANCODE_MINUS])  lightIntensity -= 1.0f * deltaTime;
        if (lightIntensity > 1.5f) lightIntensity = 1.5f;
        if (lightIntensity < 0.0f) lightIntensity = 0.0f;

        glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        
        glUniform3f(lightDirLoc, -0.5f, -1.0f, -0.3f); 
        glUniform1f(lightIntLoc, lightIntensity);
        

        float projection[16];
        mat4_perspective(radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f, projection);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);

        float view[16];
        mat4_lookAt(cameraPos, vec3_add(cameraPos, cameraFront), cameraUp, view);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);

        // talaj
        glBindTexture(GL_TEXTURE_2D, texGround);
        glBindVertexArray(groundVAO);
        float modelGround[16];
        mat4_identity(modelGround);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelGround);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // piramis
        glBindTexture(GL_TEXTURE_2D, texPyramid);
        glBindVertexArray(pyramidVAO);
        float modelPyramid[16];
        mat4_rotateY(modelPyramid, radians(45.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelPyramid);
        glDrawArrays(GL_TRIANGLES, 0, 18);

        SDL_GL_SwapWindow(window);
    }

    glDeleteVertexArrays(1, &pyramidVAO);
    glDeleteBuffers(1, &pyramidVBO);
    glDeleteVertexArrays(1, &groundVAO);
    glDeleteBuffers(1, &groundVBO);
    glDeleteProgram(shaderProgram);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}