#include "scene.h"
#include "stb_image.h"
#include <stddef.h>
#include <stdlib.h>


    float groundVertices[] = {
        -5.0f, 0.0f, -5.0f,    0.0f, 5.0f,    0.0f, 1.0f, 0.0f, 
         5.0f, 0.0f, -5.0f,    5.0f, 5.0f,    0.0f, 1.0f, 0.0f,
         5.0f, 0.0f,  5.0f,    5.0f, 0.0f,    0.0f, 1.0f, 0.0f,
         5.0f, 0.0f,  5.0f,    5.0f, 0.0f,    0.0f, 1.0f, 0.0f,
        -5.0f, 0.0f,  5.0f,    0.0f, 0.0f,    0.0f, 1.0f, 0.0f,
        -5.0f, 0.0f, -5.0f,    0.0f, 5.0f,    0.0f, 1.0f, 0.0f
    };
    float waterVertices[] = {
        -50.0f, -0.05f, -50.0f,   0.0f, 25.0f,   0.0f, 1.0f, 0.0f, 
         50.0f, -0.05f, -50.0f,  25.0f, 25.0f,   0.0f, 1.0f, 0.0f,
         50.0f, -0.05f,  50.0f,  25.0f,  0.0f,   0.0f, 1.0f, 0.0f,
         50.0f, -0.05f,  50.0f,  25.0f,  0.0f,   0.0f, 1.0f, 0.0f,
        -50.0f, -0.05f,  50.0f,   0.0f,  0.0f,   0.0f, 1.0f, 0.0f,
        -50.0f, -0.05f, -50.0f,   0.0f, 25.0f,   0.0f, 1.0f, 0.0f
    };
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

    float uiVertices[] = {
        -0.6f,  0.6f,        0.0f, 1.0f,
        -0.6f, -0.6f,        0.0f, 0.0f,
         0.6f, -0.6f,        1.0f, 0.0f,
        -0.6f,  0.6f,        0.0f, 1.0f,
         0.6f, -0.6f,        1.0f, 0.0f,
         0.6f,  0.6f,        1.0f, 1.0f
    };

void drawMesh(unsigned int vao, unsigned int texture, int vertexCount, int modelLoc, float* modelMatrix) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMatrix);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

Scene initScene(void)
{
    Scene scene;

    scene.mainShader = createProgram("assets/shaders/main.vert", "assets/shaders/main.frag");
    scene.waterShader = createProgram("assets/shaders/water.vert", "assets/shaders/water.frag");
    scene.skyboxShader = createProgram("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
    scene.uiShader = createProgram("assets/shaders/ui.vert", "assets/shaders/ui.frag");

    scene.pyramidVertexCount = 0;
    float* objVertices = loadOBJ("assets/models/piramid.obj", &scene.pyramidVertexCount);
    unsigned int objVBO;
    
    if (objVertices != NULL) {
        setupMesh(objVertices, scene.pyramidVertexCount * 8 * sizeof(float), &scene.pyramidVAO, &objVBO);
        free(objVertices);
    } else {
        scene.pyramidVAO = 0;
    }

    // ground
    unsigned int groundVBO;
    setupMesh(groundVertices, sizeof(groundVertices), &scene.groundVAO, &groundVBO);

    // water
    unsigned int waterVBO;
    setupMesh(waterVertices, sizeof(waterVertices), &scene.waterVAO, &waterVBO);

    // skybox
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &scene.skyboxVAO); 
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(scene.skyboxVAO); 
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // ui
    unsigned int uiVAO, uiVBO;
    glGenVertexArrays(1, &scene.uiVAO); 
    glGenBuffers(1, &uiVBO);
    glBindVertexArray(scene.uiVAO); 
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
    scene.cubemapTexture = loadCubemap(faces);

    stbi_set_flip_vertically_on_load(true);
    scene.texPyramid = loadTexture("assets/textures/piramid.jpg"); 
    scene.texGround = loadTexture("assets/textures/ground.jpg"); 
    scene.texWater = loadTexture("assets/textures/water.jpg");
    scene.texSugo = loadTexture("assets/textures/sugo.png");

    glUseProgram(scene.skyboxShader);
    glUniform1i(glGetUniformLocation(scene.skyboxShader, "skybox"), 0);

    return scene;
}

void renderScene(Scene* scene, Camera* camera, float lightIntensity, float currentFrame, float windowWidth, float windowHeight, bool showSugo) {
    glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    float projection[16];
    mat4_perspective(radians(45.0f), windowWidth / windowHeight, 0.1f, 100.0f, projection);
    
    float view[16];
    mat4_lookAt(camera->position, vec3_add(camera->position, camera->front), camera->up, view);

    float modelMatrix[16];

    // ground + obj
    glUseProgram(scene->mainShader);
    int modelLoc = glGetUniformLocation(scene->mainShader, "model");
    int viewLoc = glGetUniformLocation(scene->mainShader, "view");
    int projLoc = glGetUniformLocation(scene->mainShader, "projection");
    int lightDirLoc = glGetUniformLocation(scene->mainShader, "lightDir");
    int lightIntLoc = glGetUniformLocation(scene->mainShader, "lightIntensity");

    glUniform3f(lightDirLoc, -0.5f, -1.0f, 1.0f); 
    glUniform1f(lightIntLoc, lightIntensity);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);

    mat4_identity(modelMatrix);
    drawMesh(scene->groundVAO, scene->texGround, 6, modelLoc, modelMatrix);

    if (scene->pyramidVertexCount > 0) {
        mat4_identity(modelMatrix);
        drawMesh(scene->pyramidVAO, scene->texPyramid, scene->pyramidVertexCount, modelLoc, modelMatrix);
    }

    // water
    glUseProgram(scene->waterShader);
    int wModelLoc = glGetUniformLocation(scene->waterShader, "model");
    int wViewLoc = glGetUniformLocation(scene->waterShader, "view");
    int wProjLoc = glGetUniformLocation(scene->waterShader, "projection");
    int wLightDirLoc = glGetUniformLocation(scene->waterShader, "lightDir");
    int wLightIntLoc = glGetUniformLocation(scene->waterShader, "lightIntensity");
    int wTimeLoc = glGetUniformLocation(scene->waterShader, "time");

    glUniform3f(wLightDirLoc, -0.5f, -1.0f, 1.0f); 
    glUniform1f(wLightIntLoc, lightIntensity);
    glUniformMatrix4fv(wProjLoc, 1, GL_FALSE, projection);
    glUniformMatrix4fv(wViewLoc, 1, GL_FALSE, view);
    glUniform1f(wTimeLoc, currentFrame); 

    mat4_identity(modelMatrix);
    drawMesh(scene->waterVAO, scene->texWater, 6, wModelLoc, modelMatrix);

    // skybox
    glDepthFunc(GL_LEQUAL); 
    glUseProgram(scene->skyboxShader);
    int sbViewLoc = glGetUniformLocation(scene->skyboxShader, "view");
    int sbProjLoc = glGetUniformLocation(scene->skyboxShader, "projection");
    
    glUniformMatrix4fv(sbViewLoc, 1, GL_FALSE, view);
    glUniformMatrix4fv(sbProjLoc, 1, GL_FALSE, projection);
    
    glBindVertexArray(scene->skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, scene->cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

    // ui
    if (showSugo) {
        glDisable(GL_DEPTH_TEST);
        glUseProgram(scene->uiShader);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, scene->texSugo);
        
        glBindVertexArray(scene->uiVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glEnable(GL_DEPTH_TEST);
    }
}