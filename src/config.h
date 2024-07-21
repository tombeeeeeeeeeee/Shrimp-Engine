#pragma once
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include "../Utilities.h"
#include "../GeneralMacros.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern float NEARCLIP;
extern float FARCLIP;
extern float FOV;

enum Shader
{
    defaultPBR = 0,
    light,
    hdrBloom,
    irradiance,
    prefilter,
    brdf,
    skybox,
    downSample,
    upSample,
    count,
};