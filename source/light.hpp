#ifndef LIGHT_H
#define LIGHT_H

#include "lightsetting.hpp"
#include "utils.hpp"
#include "Matrix.h"

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
        void MoveLight(KeyboardState* keyboard);
        void ChangeColor(KeyboardState* keyboard);
        GLuint VBO;
        GLuint CBO;
        GLuint IBO;
        int colorCounter;
        Vector position;
        Vector color;

    public:
        LightSettings settings;
        Light(LightSettings settings, Vector position, Vector color);
        void Update(KeyboardState* keyboard);
        void LightUpScene(GLuint shaderProgram);        
        void Reset();
};

#endif /* LIGHT_H */