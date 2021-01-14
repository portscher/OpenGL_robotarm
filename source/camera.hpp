#ifndef CAMERA_H
#define CAMERA_H

#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include "utils.hpp"
#include "Matrix.h"

extern float winWidth;
extern float winHeight;

using namespace std;

class Camera
{
private:
    float currentPosition[3];
    float direction[3];
    float up[3];
    float xAngle;
    float yAngle;
    float fieldOfView;

    void MoveUp(float speed);

    void MoveDown(float speed);

    void MoveLeft(float speed);

    void MoveRight(float speed);

    void LookAt(float *target);

public:
    explicit Camera(Vector pos);

    void UpdatePosition(KeyboardState *keyboardState, MouseState *mouseState);

    void UpdateView();

    void UpdateZoom(ScrollWheelState *state);

    void Shoot(GLuint program);

    float projectionMatrix[16];
    float viewMatrix[16];
};

#endif /* CAMERA_H */
