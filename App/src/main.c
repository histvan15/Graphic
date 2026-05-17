#define SDL_MAIN_HANDLED
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "stb_image.h"
#include "engine.h"
#include "camera.h"
#include "window.h"

int main(int argc, char* argv[]) {
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float lightIntensity = 1.0f;
    Camera camera = initCamera((vec3){0.0f, 1.0f, 4.0f});

    AppWindow appWin;
    if (!initWindow(&appWin, "3D Beadando", 800, 600)) {
        return -1;
    }

    unsigned int shaderProgram = createProgram("assets/shaders/main.vert", "assets/shaders/main.frag");
    unsigned int waterProgram  = createProgram("assets/shaders/water.vert", "assets/shaders/water.frag");
    unsigned int skyboxProgram = createProgram("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
    unsigned int uiProgram     = createProgram("assets/shaders/ui.vert", "assets/shaders/ui.frag");

    // obj 
    int objVertexCount = 0;
    float* objVertices = loadOBJ("assets/models/piramid.obj", &objVertexCount);
    unsigned int objVBO, objVAO;
    if (objVertices != NULL) {
        setupMesh(objVertices, objVertexCount * 8 * sizeof(float), &objVAO, &objVBO);
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
    setupMesh(groundVertices, sizeof(groundVertices), &groundVAO, &groundVBO);

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
    setupMesh(waterVertices, sizeof(waterVertices), &waterVAO, &waterVBO);

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
    glGenVertexArrays(1, &skyboxVAO); 
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO); 
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // ui
    float uiVertices[] = {
        -0.6f,  0.6f,        0.0f, 1.0f,
        -0.6f, -0.6f,        0.0f, 0.0f,
         0.6f, -0.6f,        1.0f, 0.0f,
        -0.6f,  0.6f,        0.0f, 1.0f,
         0.6f, -0.6f,        1.0f, 0.0f,
         0.6f,  0.6f,        1.0f, 1.0f
    };

    unsigned int uiVAO, uiVBO;
    glGenVertexArrays(1, &uiVAO); 
    glGenBuffers(1, &uiVBO);
    glBindVertexArray(uiVAO); 
    glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uiVertices), uiVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); 
    glEnableVertexAttribArray(1);

    stbi_set_flip_vertically_on_load(false); 
    char* faces[] = {
        "assets/textures/skybox/right.png", "assets/textures/skybox/left.png",
        "assets/textures/skybox/top.png", "assets/textures/skybox/bottom.png",
        "assets/textures/skybox/front.png", "assets/textures/skybox/back.png"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    stbi_set_flip_vertically_on_load(true);

    unsigned int texPyramid = loadTexture("assets/textures/piramid.jpg"); 
    unsigned int texGround = loadTexture("assets/textures/ground.jpg"); 
    unsigned int texWater = loadTexture("assets/textures/water.jpg");
    unsigned int texSugo = loadTexture("assets/textures/sugo.png");

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

    bool showSugo = false;

    while (running) {
        float currentFrame = SDL_GetTicks() / 1000.0f;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;

            // sugo toggle F1
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_F1) {
                    showSugo = !showSugo;
                }
            }

            if (event.type == SDL_MOUSEMOTION) {
                float xoffset = event.motion.xrel * 0.1f;
                float yoffset = -event.motion.yrel * 0.1f;
                processMouseInput(&camera, xoffset, yoffset);
            }
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);

        // movement
        processKeyboardInput(&camera, state, deltaTime);
        if(state[SDL_SCANCODE_ESCAPE]) running = 0;
        
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
        mat4_lookAt(camera.position, vec3_add(camera.position, camera.front), camera.up, view);

        // ground + obj
        glUseProgram(shaderProgram);
        glUniform3f(lightDirLoc, -0.5f, -1.0f, 1.0f); 
        glUniform1f(lightIntLoc, lightIntensity);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);

        float modelMatrix[16];
        mat4_identity(modelMatrix);
        drawMesh(groundVAO, texGround, 6, modelLoc, modelMatrix);

        if (objVertexCount > 0) {
            mat4_identity(modelMatrix);
            drawMesh(objVAO, texPyramid, objVertexCount, modelLoc, modelMatrix);
        }

        // water
        glUseProgram(waterProgram);
        glUniform3f(wLightDirLoc, -0.5f, -1.0f, 1.0f); 
        glUniform1f(wLightIntLoc, lightIntensity);
        glUniformMatrix4fv(wProjLoc, 1, GL_FALSE, projection);
        glUniformMatrix4fv(wViewLoc, 1, GL_FALSE, view);
        glUniform1f(wTimeLoc, currentFrame); 

        mat4_identity(modelMatrix);
        drawMesh(waterVAO, texWater, 6 , wModelLoc, modelMatrix);

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

        if (showSugo) {
            glDisable(GL_DEPTH_TEST);
            glUseProgram(uiProgram);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texSugo);
            
            glBindVertexArray(uiVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            
            glEnable(GL_DEPTH_TEST);
        }

        SDL_GL_SwapWindow(appWin.handle);
    }

    cleanupWindow(&appWin);

    return 0;
}