#ifndef LIMB_H
#define LIMB_H

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "utils.hpp"
#include "Matrix.h"
#include "Vector.hpp"

class Arm;

class Limb
{
private:
    int ID;
    Arm *arm;

    std::string filename;
    std::string texture;

    GLuint VAO;
    GLuint NBO; // normals

    float rotationX;
    float rotationY;
    float rotationZ;

    int angle;

    float _offset; // only the y-offset

    float position[3];

    GLuint TextureID;

    // internal is used for internal transformations like scale
    float internal[16];
    // transformation has rotations + translations
    float transformation[16];
    float model[16];

public:
    Limb(Arm *arm, int ID, std::string filename, std::string texture, float position[3], float scale);

    void setRotation(int axis, float deg);

    float getRotation(int axis);

    void getTransformation(float *transformation);

    void setAngle(int deg);

    void update(float *transformation);

    void display(GLint program);

};

#endif
