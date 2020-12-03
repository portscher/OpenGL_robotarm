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
#include <iostream>
#include <cstdlib>

/* OpenGL includes */
#include <GL/glew.h>
#include <GLFW/glfw3.h>

/* Local includes */
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
const float FIRST_LIMB_HEIGHT = 2.5f;
float COLOUR1[3] = {0.0f, 0.3f, 0.5f};
const float SECOND_LIMB_LENGTH = 2.5f;
float COLOUR2[3] = {0.3f, 0.5f, 0.0f};
const float THIRD_LIMB_LENGTH = 2.0f;
float COLOUR3[3] = {0.5f, 0.0f, 0.3f};

GLuint ShaderProgram;

KeyboardState keyboard = {
        .up = 0,
        .down = 0,
        .left = 0,
        .right = 0,
        .currentLimb = 0,
        .reset = 0,
};

ScrollWheelState scrollWheel
        {
                .zoom = 45.0f,
        };

MouseState mouse
        {
                .lastX = winWidth / 2,
                .lastY = winHeight / 2,
                .firstMouse = 1,
                .xAngle = 0.0f,
                .yAngle = 0.0f,
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

void Display(Arm arm, Camera cam)
{
    /* Clear window; color specified in 'Initialize()' */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Associate program with uniform shader matrices */
    GLint projectionUniform = glGetUniformLocation(ShaderProgram, "ProjectionMatrix");
    if (projectionUniform == -1)
    {
        fprintf(stderr, "Could not bind uniform ProjectionMatrix\n");
        exit(-1);
    }
    glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, cam.projectionMatrix);

    GLint ViewUniform = glGetUniformLocation(ShaderProgram, "ViewMatrix");
    if (ViewUniform == -1)
    {
        fprintf(stderr, "Could not bind uniform ViewMatrix\n");
        exit(-1);
    }
    glUniformMatrix4fv(ViewUniform, 1, GL_TRUE, cam.viewMatrix);

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
void Initialize(Camera cam)
{
    /* Set background (clear) color to gray */
    glClearColor(0.1, 0.1, 0.1, 0.0);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    /* Setup shaders and shader program */
    ShaderProgram = glCreateProgram();
    CreateShaderProgram(ShaderProgram);

    /* Initialize project/view matrices */
    SetIdentityMatrix(cam.projectionMatrix);
    SetIdentityMatrix(cam.viewMatrix);

    /* Set projection transform */
    float aspect = winWidth / winHeight;
    float nearPlane = 1.0;
    float farPlane = 50.0;
    SetPerspectiveMatrix(45.0, aspect, nearPlane, farPlane, cam.projectionMatrix); /* build projection matrix */

    /* Set viewing transform */
    SetTranslation(0.0, -5.0, -20.0, cam.viewMatrix); /* translation of the camera */
    float RotationMatrix[16];
    SetRotationX(15.0, RotationMatrix); /* small rotation of the camera, to look at the center of the scene */
    MultiplyMatrix(RotationMatrix, cam.viewMatrix, cam.viewMatrix); /* assemble View matrix */
}


/******************************************************************
*
* Resize
*
* This function is called when the window size is changed
* to update the viewport dimension accordingly
*
* width, height = new dimension of the window after resize
*******************************************************************/

void Resize(GLFWwindow *window, int width, int height)
{
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
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{

    if (key == GLFW_KEY_W)
    {  // forwards
        if (action == GLFW_PRESS)
        {
            keyboard.up = 1;
        } else if (action == GLFW_RELEASE)
        {
            keyboard.up = 0;
        }
    } else if (key == GLFW_KEY_S)
    { // backwards
        if (action == GLFW_PRESS)
        {
            keyboard.down = 1;
        } else if (action == GLFW_RELEASE)
        {
            keyboard.down = 0;
        }
    } else if (key == GLFW_KEY_A)
    { // left
        if (action == GLFW_PRESS)
        {
            keyboard.left = 1;
        } else if (action == GLFW_RELEASE)
        {
            keyboard.left = 0;
        }
    } else if (key == GLFW_KEY_D)
    { // right
        if (action == GLFW_PRESS)
        {
            keyboard.right = 1;
        } else if (action == GLFW_RELEASE)
        {
            keyboard.right = 0;
        }
    } else if ((key == GLFW_KEY_1 || key == GLFW_KEY_2 || key == GLFW_KEY_3 || key == GLFW_KEY_0)
               && action == GLFW_PRESS)
    {
        keyboard.currentLimb = key - 48; // GLFW_KEY_0 = 48
    } else if (key == GLFW_KEY_R)
    {
        if (action == GLFW_PRESS)
        {
            keyboard.reset = 1;
        } else if (action == GLFW_RELEASE)
        {
            keyboard.reset = 0;
        }
    } else if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
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
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    scrollWheel.zoom -= (float) yoffset;
    // limit zoom to a minimum factor of 1.0
    if (scrollWheel.zoom < 1.0f)
    {
        scrollWheel.zoom = 1.0f;
    }

    // limit zoom to a maximum factor of 45.0
    if (scrollWheel.zoom > 45.0f)
    {
        scrollWheel.zoom = 45.0f;
    }
}

/**
 * @brief Provides a callback for receiving the state of the mouse.
 * 
 * @param window The actual window
 * @param xpos The x-position of the mouse
 * @param ypos The y-position of the mouse.
 * @remarks Based on the article at https://learnopengl.com/Getting-started/Camera
 */
void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (mouse.firstMouse)
    {
        mouse.lastX = xpos;
        mouse.lastY = ypos;
        mouse.firstMouse = false;
    }

    float xOffset = xpos - mouse.lastX;
    float yOffset = mouse.lastY - ypos;
    mouse.lastX = xpos;
    mouse.lastY = ypos;

    float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    mouse.xAngle += xOffset;
    mouse.yAngle += yOffset;

    if (mouse.xAngle > 89.0f)
    {
        mouse.xAngle = 89.0f;
    }

    if (mouse.yAngle < -89.0f)
    {
        mouse.yAngle = -89.0f;
    }
}

/******************************************************************
*
* main
*
* Main function to setup GLUT, GLEW, and enter rendering loop
*
*******************************************************************/

int main(int argc, char **argv)
{

    /* Initialize GLFW and create a window */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(winWidth, winHeight, "PS3 - Transformations", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    /* Link callback functions */
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, Resize);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetCursorPosCallback(window, mouseCallback);

    /* Initialize timer */
    glfwSetTime(0.0f);

    /* Initialize GL extension wrangler */
    glewExperimental = true;
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
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
    arm.addLimb(THICKNESS, FIRST_LIMB_HEIGHT, COLOUR1);
    arm.addLimb(THICKNESS, SECOND_LIMB_LENGTH, COLOUR2);
    arm.addLimb(THICKNESS, THIRD_LIMB_LENGTH, COLOUR3);

    /* Rendering loop */
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        /* Update scene */
        arm.update(&keyboard);

        camera.UpdatePosition(&keyboard, &mouse);
        camera.UpdateZoom(&scrollWheel);

        /* Draw scene */
        Display(arm, camera);
    }

    /* Close window */
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}