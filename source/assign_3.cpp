/******************************************************************
*
* assign_1.cpp
*
* Interactive Graphics and Simulation Group
* Institute of Computer Science
* University of Innsbruck
*
*******************************************************************/


/* Standard includes */
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include <cstdlib>

/* OpenGL includes */
#include <GL/glew.h>
#include <GLFW/glfw3.h>

/* Local includes */
#include "LoadShader.h"    /* Loading function for shader code */
#include "Matrix.h"        /* Functions for matrix handling */
#include "arm.hpp"
#include "utils.hpp"
#include "camera.hpp"

/* Window parameters */
float winWidth = 1000.0f;
float winHeight = 800.0f;

/* window */
GLFWwindow *window;

GLuint defaultVAO;

const float THICKNESS = .4;
const float FIRST_LIMB_HEIGHT = 1.5f;
const float SECOND_LIMB_LENGTH = 1.5f;
const float THIRD_LIMB_LENGTH = 1.0f;

GLuint ShaderProgram;

/* Matrices for uniform variables in vertex shader */
float ProjectionMatrix[16];             /* Perspective projection matrix */
float ViewMatrix[16];                   /* Camera view matrix */

KeyboardState keyboard = {
    .up = 0,
    .down = 0,
    .left = 0,
    .right = 0,
    .currentLimb = 0,
};

/******************************************************************
*
* Display
*
* This function is called when the content of the window needs to be
* drawn/redrawn. It has been specified through 'glutDisplayFunc()';
* Enable vertex attributes, create binding between C program and
* attribute name in shader, provide data for uniform variables
*
*******************************************************************/

void Display(Arm arm, Camera cam) {
    /* Clear window; color specified in 'Initialize()' */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Associate program with uniform shader matrices */
    GLint projectionUniform = glGetUniformLocation(ShaderProgram, "ProjectionMatrix");
    if (projectionUniform == -1) {
        fprintf(stderr, "Could not bind uniform ProjectionMatrix\n");
        exit(-1);
    }
    glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, ProjectionMatrix);

    GLint ViewUniform = glGetUniformLocation(ShaderProgram, "ViewMatrix");
    if (ViewUniform == -1) {
        fprintf(stderr, "Could not bind uniform ViewMatrix\n");
        exit(-1);
    }
    glUniformMatrix4fv(ViewUniform, 1, GL_TRUE, ViewMatrix);

    arm.display(ShaderProgram);

    /* Swap between front and back buffer */
    glfwSwapBuffers(window);
}

/******************************************************************
*
* Initialize
*
* This function is called to initialize rendering elements, setup
* vertex buffer objects, and to setup the vertex and fragment shader;
* meshes are loaded from files in OBJ format; data is copied from
* structures into vertex and index arrays
*
*******************************************************************/
void Initialize(Camera cam) {
    /* Set background (clear) color to blue */
    glClearColor(0.0, 0.0, 0.4, 0.0);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    /* Setup shaders and shader program */
    ShaderProgram = glCreateProgram();
    CreateShaderProgram(ShaderProgram);

    /* Initialize project/view matrices */
    SetIdentityMatrix(ProjectionMatrix);
    SetIdentityMatrix(ViewMatrix);

    /* Set projection transform */
    float aspect = winWidth / winHeight;
    float nearPlane = 1.0;
    float farPlane = 50.0;
    SetPerspectiveMatrix(45.0, aspect, nearPlane, farPlane, ProjectionMatrix); /* build projection matrix */

    /* Set viewing transform */
    SetTranslation(0.0, -5.0, -20.0, ViewMatrix); /* translation of the camera */
    float RotationMatrix[16];
    SetRotationX(15.0, RotationMatrix); /* small rotation of the camera, to look at the center of the scene */
    MultiplyMatrix(RotationMatrix, ViewMatrix, ViewMatrix); /* assemble View matrix */

//    cam.UpdateView();
}


/******************************************************************
*
* Resize
*
* This function is called when the window size is changed
* to update the viewport dimension accordingly
*
* width, height = new dimesnion of the window after resize
*******************************************************************/

void Resize(GLFWwindow *window, int width, int height) {
    /* update viewport with new dimensions */
    glViewport(0, 0, width, height);
}


/******************************************************************
*
* keyCallback
*
* This function is called when a keyboard action is detected
*
* key = id of the key activated
* action = id of the action (pressed, released, maintained ...)
*******************************************************************/
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_W) {  // forwards
        if (action == GLFW_PRESS) {
            keyboard.up = 1;
        } else if (action == GLFW_RELEASE) {
            keyboard.up = 0;
        }
    } else if (key == GLFW_KEY_S) { // backwards
        if (action == GLFW_PRESS) {
            keyboard.down = 1;
        } else if (action == GLFW_RELEASE) {
            keyboard.down = 0;
        }
    } else if (key == GLFW_KEY_A) { // left
        if (action == GLFW_PRESS) {
            keyboard.left = 1;
        } else if (action == GLFW_RELEASE) {
            keyboard.left = 0;
        }
    } else if (key == GLFW_KEY_D) { // right
        if (action == GLFW_PRESS) {
            keyboard.right = 1;
        } else if (action == GLFW_RELEASE) {
            keyboard.right = 0;
        }
    } else if ((key == GLFW_KEY_1 || key == GLFW_KEY_2 || key == GLFW_KEY_3 || key == GLFW_KEY_0)
            && action == GLFW_PRESS) {
        keyboard.currentLimb = key - 48; // GLFW_KEY_0 = 48
    } else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        std::cout << "Bye!" << std::endl;
        exit(0);
    }

}


/******************************************************************
*
* scrollCallback
*
* This function is called when mouse scrolling is detected
*
* yoffset = value of the scrolling along Y axis
*******************************************************************/
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    std::cout << "scrolling Y offest = " << yoffset << std::endl;
}

/******************************************************************
*
* main
*
* Main function to setup GLUT, GLEW, and enter rendering loop
*
*******************************************************************/

int main(int argc, char **argv) {

    /* Initialize GLFW and create a window */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(winWidth, winHeight, "PS3 - Transformations", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    /* Link callback functions */
    glfwSetFramebufferSizeCallback(window, Resize);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetScrollCallback(window, scrollCallback);

    /* Initialize timer */
    glfwSetTime(0.0f);

    /* Initialize GL extension wrangler */
    glewExperimental = true;
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    glGenVertexArrays(1, &defaultVAO);
    glBindVertexArray(defaultVAO);

    Camera camera;

    /* Setup scene and rendering parameters */
    Initialize(camera);

    Arm arm;
    arm.addLimb(THICKNESS, FIRST_LIMB_HEIGHT);
    arm.addLimb(THICKNESS, SECOND_LIMB_LENGTH);
    arm.addLimb(THICKNESS, SECOND_LIMB_LENGTH);

    /* Rendering loop */
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        /* Update scene */
        arm.update(&keyboard);

        camera.UpdateView(ProjectionMatrix, ViewMatrix);
        camera.UpdatePosition(&keyboard);
        
        /* Draw scene */
        Display(arm, camera);
    }

    /* Close window */
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
