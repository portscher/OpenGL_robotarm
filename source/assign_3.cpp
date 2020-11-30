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

void Display(Arm arm) {
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
void Initialize() {
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

/**
 * @brief Updates the position of the camera object.
 * 
 */
void UpdateCameraPosition() {
    float cameraSpeed = 0.2;
    float temp[3];
    if (keyboard.up)
    {
        ScalarMultiplication(cameraSpeed, cam.front, 3, temp);
        Add(cam.currentPosition, temp, 3, cam.currentPosition);
    }

    if (keyboard.down)
    {
        ScalarMultiplication(cameraSpeed, cam.front, 3, temp);
        Substract(cam.currentPosition, temp, 3, cam.currentPosition);
    }

    if (keyboard.left)
    {
        CrossProduct(cam.front, cam.up, temp);
        NormalizeVector(temp, 3, temp);
        ScalarMultiplication(cameraSpeed * 0.8, temp, 3, temp);
        Substract(cam.currentPosition, temp, 3, cam.currentPosition);
    }

    if (keyboard.right)
    {
        CrossProduct(cam.front, cam.up, temp);
        NormalizeVector(temp, 3, temp);
        ScalarMultiplication(cameraSpeed * 0.8, temp, 3, temp);
        Add(cam.currentPosition, temp, 3, cam.currentPosition);
    }
}

void UpdateCameraView()
{
    float target[3];
    UpdateCameraPosition();
    cam.front[0] = cos(ToRadian(cam.xAngle)) * sin(ToRadian(cam.yAngle));
    cam.front[1] = sin(ToRadian(cam.xAngle));
    cam.front[2] = cos(ToRadian(cam.xAngle)) * sin(ToRadian(cam.yAngle));
    NormalizeVector(cam.front, 3, cam.front);
    Add(cam.currentPosition, cam.front, 3, target);

    printf("pos: [%g,%g, %g]\nfront: [%g, %g, %g]\nlookingat: [%g, %g, %g]\n",
    cam.currentPosition[0], cam.currentPosition[1], cam.currentPosition[2],
    cam.front[0], cam.front[1], cam.front[2],
    target[0], target[1], target[2]);

    LookAt(cam.currentPosition, target, cam.up, cam.viewMatrix);

    float aspect = winWidth / winHeight;
    float nearPlane = 1.0;
    float farPlane = 50.0;
    SetPerspectiveMatrix(cam.fieldOfView, aspect, nearPlane, farPlane, cam.projectionMatrix);
}

/**
 * @brief Calculates the view matrix of the camera.
 * 
 * @param position The current position of the camera.
 * @param target The target to which the camera is pointing.
 * @param upVector The up vector of the camera.
 * @param result The resulting view matrix.
 */
void LookAt(float* position, float* target, float* upVector, float* result)
{
    float forward[3];
    Substract(position, target, 3, forward);
    NormalizeVector(forward, 3, forward);

    float left[3];
    CrossProduct(upVector, forward, left);
    NormalizeVector(left, 3, left);

    float up[3];
    CrossProduct(forward, left, up);

    float matrix[16];
    SetIdentityMatrix(matrix);
    
    matrix[0] = left[0];
    matrix[4] = left[1];
    matrix[8] = left[2];
    matrix[1] = up[0];
    matrix[5] = up[1];
    matrix[9] = up[2];
    matrix[2] = forward[0];
    matrix[6] = forward[1];
    matrix[10] = forward[2];

    matrix[12] = -left[0] * position[0] - left[1] * position[1] - left[2] * position[2];
    matrix[13] = -up[0] * position[0] - up[1] * position[1] - up[2] * position[2];
    matrix[14] = -forward[0] * position[0] - forward[1] * position[1] - forward[2] * position[2];

    memcpy(result, matrix, 16 * sizeof(float));
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

    /* Setup scene and rendering parameters */
    Initialize();

    Arm arm;
    arm.addLimb(THICKNESS, FIRST_LIMB_HEIGHT);
    arm.addLimb(THICKNESS, SECOND_LIMB_LENGTH);
    arm.addLimb(THICKNESS, SECOND_LIMB_LENGTH);

    /* Rendering loop */
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        /* Update scene */
        arm.update(&keyboard);

        /* Draw scene */
        Display(arm);
    }

    /* Close window */
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
