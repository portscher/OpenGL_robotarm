#ifndef LIMB_H
#define LIMB_H

#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "utils.hpp"
#include "Matrix.h"

// const float THICKNESS = 0.3f;
// const float FIRST_LIMB_HEIGHT = 2.5f;
// const float FIRST_LIMB_Y_TRANSLATION = 2.25f;
// const float SECOND_LIMB_LENGTH = 1.5f;
// const float SECOND_LIMB_X_TRANSLATION = SECOND_LIMB_LENGTH / 2 + 0.45f;
// const float THIRD_LIMB_LENGTH = 1.0f;

using namespace std;

class Limb
{
private:
    int ID;
    GLint VAO;

    int rotationX;
    int rotationY;
    int rotationZ;

    int angle;

    // TODO make this a vector. and all 3xarrays
    float _offset; // for now, it is only the y-offset

    float position[3];
    float legth;
    float width;

    // internal is used for internal transformations like scale
    float internal[16];
    // transformation has rotations + translations
    float transformation[16];
    float model[16];

public:
    Limb(int ID, float position[3], float size[3]);

    void setRotation(int axis, float grad);
    float getRotation(int axis);
    void setAngle(int grad);
    void update();
    void display(GLint program);
    float offset();
};

#endif /* LIMB_H */
