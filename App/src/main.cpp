#define SDL_MAIN_HANDLED
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n" 
    "out vec2 TexCoord;\n" 
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   TexCoord = aTexCoord;\n"
    "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(texture1, TexCoord);\n"
    "}\n\0";

// kamera beállítások
glm::vec3 cameraPos   = glm::vec3(0.0f, 1.0f,  4.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float yaw   = -90.0f;
float pitch = -10.0f; 

int main(int argc, char* argv[]) {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow("Sziget Projekt - A Fold es Piramis", 
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

    float pyramidVertices[] = { 
        // alap
        -0.5f, 0.0f, -0.5f,   0.0f, 0.0f, 
         0.5f, 0.0f, -0.5f,   1.0f, 0.0f,  
         0.5f, 0.0f,  0.5f,   1.0f, 1.0f, 
         0.5f, 0.0f,  0.5f,   1.0f, 1.0f,  
        -0.5f, 0.0f,  0.5f,   0.0f, 1.0f,  
        -0.5f, 0.0f, -0.5f,   0.0f, 0.0f, 
        // elülső lap
        -0.5f, 0.0f,  0.5f,   0.0f, 0.0f,  
         0.5f, 0.0f,  0.5f,   1.0f, 0.0f,  
         0.0f, 1.0f,  0.0f,   0.5f, 1.0f,  
        // hátsó lap
         0.5f, 0.0f, -0.5f,   0.0f, 0.0f,
        -0.5f, 0.0f, -0.5f,   1.0f, 0.0f,
         0.0f, 1.0f,  0.0f,   0.5f, 1.0f,
        // bal lap
        -0.5f, 0.0f, -0.5f,   0.0f, 0.0f,
        -0.5f, 0.0f,  0.5f,   1.0f, 0.0f,
         0.0f, 1.0f,  0.0f,   0.5f, 1.0f,
        // jobb lap
         0.5f, 0.0f,  0.5f,   0.0f, 0.0f,
         0.5f, 0.0f, -0.5f,   1.0f, 0.0f, 
         0.0f, 1.0f,  0.0f,   0.5f, 1.0f 
    };
    unsigned int pyramidVBO, pyramidVAO;
    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);
    glBindVertexArray(pyramidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    float groundVertices[] = {
        // (X, Y, Z)
        -20.0f, 0.0f, -20.0f,     0.0f, 10.0f, 
         20.0f, 0.0f, -20.0f,    10.0f, 10.0f,
         20.0f, 0.0f,  20.0f,    10.0f,  0.0f,
         20.0f, 0.0f,  20.0f,    10.0f,  0.0f,
        -20.0f, 0.0f,  20.0f,     0.0f,  0.0f,
        -20.0f, 0.0f, -20.0f,     0.0f, 10.0f
    };
    unsigned int groundVBO, groundVAO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned int texPyramid, texGround;

    // 1. Piramis textúra
    glGenTextures(1, &texPyramid);
    glBindTexture(GL_TEXTURE_2D, texPyramid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unsigned char *data1 = stbi_load("C:\\Users\\herin\\Desktop\\Egyetem\\4_felev\\Graphic\\App\\textures\\piramid.jpg", &width, &height, &nrChannels, 0);
    if (data1) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else { std::cout << "Hiba a piramis textura betoltesekor!" << std::endl; }
    stbi_image_free(data1);
    
    glGenTextures(1, &texGround);
    glBindTexture(GL_TEXTURE_2D, texGround);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unsigned char *data2 = stbi_load("C:\\Users\\herin\\Desktop\\Egyetem\\4_felev\\Graphic\\App\\textures\\ground.jpg", &width, &height, &nrChannels, 0);
    if (data2) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else { std::cout << "Hiba a talaj textura betoltesekor!" << std::endl; }
    stbi_image_free(data2);

    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc  = glGetUniformLocation(shaderProgram, "view");
    int projLoc  = glGetUniformLocation(shaderProgram, "projection");

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

                glm::vec3 front;
                front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                front.y = sin(glm::radians(pitch));
                front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                cameraFront = glm::normalize(front);
            }
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);
        float cameraSpeed = 2.5f * deltaTime;
        
        if (state[SDL_SCANCODE_W]) cameraPos += cameraSpeed * cameraFront;
        if (state[SDL_SCANCODE_S]) cameraPos -= cameraSpeed * cameraFront;
        if (state[SDL_SCANCODE_A]) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (state[SDL_SCANCODE_D]) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        
        if (state[SDL_SCANCODE_ESCAPE]) running = 0; 

        glClearColor(0.1f, 0.3f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glBindTexture(GL_TEXTURE_2D, texGround);
        glBindVertexArray(groundVAO);
        glm::mat4 modelGround = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelGround));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, texPyramid);
        glBindVertexArray(pyramidVAO);
        glm::mat4 modelPyramid = glm::mat4(1.0f);
        modelPyramid = glm::rotate(modelPyramid, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelPyramid));
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