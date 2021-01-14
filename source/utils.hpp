
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
    vPosition = 0, vColor = 1, vNormal = 2, vUV = 3
};

typedef struct keyboard
{
    int up;
    int down;
    int left;
    int right;
    // 0: camera, 1-3: limb
    int currentLimb;
    int lightMode;
    int lightUp;
    int lightDown;
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

void readMeshFile(string filename, float scale, GLuint *NBO, GLuint *VAO);

void SetupTexture(GLuint *TextureID, const char *filename);

void AddShader(GLuint UsedShaderProgram, const char *ShaderCode, GLenum ShaderType);

GLuint CreateShaderProgram(string vsPath, string fsPath);

float constrainAngle(float x);

void BindUniform1f(const string name, GLuint program, float val);

#endif
