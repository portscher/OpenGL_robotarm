#ifndef ARM_H
#define ARM_H

#include <cstdlib>
#include <cstdio>
#include <vector>
#include "limb.hpp"
#include "utils.hpp"

class Arm
{
private:
    std::vector<Limb *> limbs;
    GLuint VAO;
    float internal[16];
    std::string filename;

public:
    Arm();

    void addLimb(std::string filename, float offset, Vector colour);

    void update(KeyboardState *state);

    void display(GLint ShaderProgram);

    static float getCurrentRotationAt(int axis, Limb *limb);
};

#endif /* ARM_H */
