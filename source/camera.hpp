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

using namespace  std;

class Camera 
{
    private:
        void LookAt();
        void UpdatePosition();
        void MoveLeft(float);
        void MoveRight(float);
        void MoveUp(float);
        void MoveDown(float);
        float currentPosition[3];
        float front[3];
        float up[3];
        float xAngle;
        float yAngle;
        int lookingAt;
        float fieldOfView;
        float viewMatrix[16];
        float projectionMatrix[16];

    public:
        Camera();
        void UpdateView(KeyboardState *state);
};

#endif /* CAMERA_H */