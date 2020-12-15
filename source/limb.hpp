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
    std::string filename;
    std::string texture;

    Arm *arm;

    GLuint VAO;
    GLuint NBO; // normals
    GLuint CBO; // colors

    float rotationX;
    float rotationY;
    float rotationZ;

    int angle;

    // TODO make this a vector. and all 3xarrays
    float _offset; // for now, it is only the y-offset

    float position[3];
    float colour[3];

    GLuint TextureID;
    GLuint TextureUniform;

    // internal is used for internal transformations like scale
    float internal[16];
    // transformation has rotations + translations
    float transformation[16];
    float model[16];

public:
    Limb(Arm *arm, int ID, std::string filename, std::string texture, float position[3], Vector colour, float scale);

    void setRotation(int axis, float deg);

    float getRotation(int axis);

    void getTransformation(float *transformation);

    void setAngle(int deg);

    void update(float *transformation);

    void display(GLint program);

};

#endif
