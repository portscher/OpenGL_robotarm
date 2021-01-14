#ifndef ARM_H
#define ARM_H

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include "utils.hpp"
#include "limb.hpp"
#include "camera.hpp"
#include "Vector.hpp"

using namespace std;

class Limb;
class Arm
{
private:
    std::vector<Limb *> limbs;

    GLuint VAO;
    GLuint NBO; // normals

    GLuint TextureID;
    GLuint TextureUniform;

    float internal[16];
    Camera *cam;

public:
    Arm(Camera *cam);

    void addLimb(std::string filename, string texture, float offset, float scale);

    void update(KeyboardState *state);

    void display(GLint ShaderProgram);

    static float getCurrentRotationAt(int axis, Limb *limb);

    // Camera Arm::getCamera();
};

#endif /* ARM_H */
