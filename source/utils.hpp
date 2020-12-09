#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <string>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "LoadShader.h"    /* Loading function for shader code */
#include "Vector.hpp"

using namespace std;

/* Indices to vertex attributes; in this case position only */
enum DataID
{
    vPosition = 0, vColor = 1, vNormal = 2
};

typedef struct keyboard
{
    int up;
    int down;
    int left;
    int right;
    // 0: camera, 1-3: limb
    int currentLimb;
    int reset;
} KeyboardState;



typedef struct scrollWheel
{
    float zoom;
} ScrollWheelState;

typedef struct mouse
{
    float lastX;
    float lastY;
    int firstMouse;
    float xAngle;
    float yAngle;
} MouseState;

GLuint createCubeMesh(float width, float height);
GLuint createCubeMesh(float width, float height, float *colour);
void readMeshFile(string filename, float scale, Vector rgb,
        GLuint *VBO, GLuint *IBO, GLuint *CBO, GLuint *NBO, GLuint *VAO);

void AddShader(GLuint UsedShaderProgram, const char *ShaderCode, GLenum ShaderType);

void CreateShaderProgram(GLuint ShaderProgram);

float constrainAngle(float x);

#endif
