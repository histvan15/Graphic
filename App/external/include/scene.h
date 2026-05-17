#ifndef SCENE_H
#define SCENE_H

#include "engine.h"
#include "camera.h"
#include <stdbool.h>

typedef struct {
    unsigned int mainShader;
    unsigned int waterShader;
    unsigned int skyboxShader;
    unsigned int uiShader;

    unsigned int pyramidVAO;
    int pyramidVertexCount;
    unsigned int groundVAO;
    unsigned int waterVAO;
    unsigned int skyboxVAO;
    unsigned int uiVAO;


    unsigned int texPyramid;
    unsigned int texGround;
    unsigned int texWater;
    unsigned int texSugo;
    unsigned int cubemapTexture;
} Scene;

Scene initScene(void);

void renderScene(Scene* scene, Camera* camera, float lightIntensity, float currentFrame, float windowWidth, float windowHeight, bool showSugo);

#endif