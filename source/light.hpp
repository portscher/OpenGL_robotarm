#ifndef LIGHT_H
#define LIGHT_H

#include "lightsetting.hpp"
#include "utils.hpp"

#include <iostream>

/* OpenGL includes */
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

class Light
{
    private:
        void LightUp(KeyboardState* keyboard);
        void LightDown(KeyboardState* keyboard);
        

    public:
        LightSettings settings;
        Light(LightSettings settings);
        void Initialize(GLuint shaderProgram);
        void Update(KeyboardState* keyboard);
};

#endif /* LIGHT_H */