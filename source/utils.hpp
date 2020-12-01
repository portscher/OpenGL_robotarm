#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "LoadShader.h"    /* Loading function for shader code */

/* Indices to vertex attributes; in this case position only */
enum DataID {
    vPosition = 0, vColor = 1
};

typedef struct keyboard {
    int up;
    int down;
    int left;
    int right;
    // 0: camera, 1-3: limb
    int currentLimb;
    int reset;
} KeyboardState;

GLuint createCubeMesh(float width, float height, float* colour);

void AddShader(GLuint UsedShaderProgram, const char *ShaderCode, GLenum ShaderType);

void CreateShaderProgram(GLuint ShaderProgram);

void printMatrix(float* mat);

#endif