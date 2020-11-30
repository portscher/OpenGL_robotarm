#ifndef ARM_H
#define ARM_H

#include <stdlib.h>
#include <cstdio>
#include <vector>
#include "limb.hpp"
#include "utils.hpp"

using namespace  std;

class Arm
{
private:
    vector<Limb*> limbs;
    GLint VAO;
    float internal[16];
    float height;
    float width;

public:
    Arm();
    void addLimb(float width, float height, float depth);
    void update(KeyboardState *state);
    void display(GLint ShaderProgram);
    Limb getLimb(int index);
};

#endif /* ARM_H */