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

/* Number of objects in the scene */
#define nbObjects 4
/*----------------------------------------------------------------*/
/* Window parameters */
float winWidth = 1000.0f;
float winHeight = 800.0f;

/* window */
GLFWwindow *window;


float scales[nbObjects][16];
float translations[nbObjects][16];
float rotations[nbObjects][16];

typedef struct cuboid {
    const int id;
    const int parent_id; // hierarchical structure, if one limb moves, all child limbs have to move as well
    float distance;
    float transformation[16];
    float model[16];

    float scale[3];
    float translation[3];
    float current_rotation[3]; /* Variables for storing current rotation angles */
    float rotation_angle;

    GLuint VBO;
    GLuint CBO;
    GLuint IBO;
    GLuint VAO;
} Cuboid;

GLuint defaultVAO;

const float THICKNESS = 0.3f;
const float FIRST_LIMB_HEIGHT = 2.5f;
const float FIRST_LIMB_Y_TRANSLATION = 2.25f;
const float SECOND_LIMB_LENGTH = 1.5f;
const float SECOND_LIMB_X_TRANSLATION = SECOND_LIMB_LENGTH / 2 + 0.45f;
const float THIRD_LIMB_LENGTH = 1.0f;

Cuboid cuboids[nbObjects] = {
        // bottom
        {.id = 0, .distance = 0, .transformation = {0}, .model = {0}, .scale = {2.5f, 0.25f, 2.5f},
                .translation = {0}, .current_rotation = {0}},
        // first limb
        {.id = 1, .parent_id = 0, .distance = 0.2, .transformation = {0}, .model = {0}, .scale = {THICKNESS,
                                                                                               FIRST_LIMB_HEIGHT,
                                                                                               THICKNESS},
                .translation = {0.0, FIRST_LIMB_Y_TRANSLATION, .0}, .current_rotation = {0}},
        // second limb
        {.id = 2, .parent_id = 1, .distance = 0.2, .transformation = {0}, .model = {0}, .scale = {SECOND_LIMB_LENGTH,
                                                                                               THICKNESS, THICKNESS},
                .translation = {SECOND_LIMB_X_TRANSLATION, FIRST_LIMB_HEIGHT + FIRST_LIMB_Y_TRANSLATION,
                                0}, .current_rotation = {0}, .rotation_angle = 90},
        // third limb
        {.id = 3, .parent_id = 2, .distance = 0.2, .transformation = {0}, .model = {0}, .scale = {THICKNESS,
                                                                                               THIRD_LIMB_LENGTH,
                                                                                               THICKNESS},
                .translation = {SECOND_LIMB_LENGTH + 0.9f,
                                FIRST_LIMB_HEIGHT + FIRST_LIMB_Y_TRANSLATION - THIRD_LIMB_LENGTH,
                                0}, .current_rotation = {0}},
};


GLuint ShaderProgram;

/* Matrices for uniform variables in vertex shader */
float ProjectionMatrix[16];             /* Perspective projection matrix */
float ViewMatrix[16];                   /* Camera view matrix */

/* Own rotation matrices for each object */
float RotationMatrixAnimX[nbObjects][16];
float RotationMatrixAnimY[nbObjects][16];
float RotationMatrixAnimZ[nbObjects][16];
float RotationMatrixAnim[nbObjects][16];

/* Reference time for animation */
double oldTime = 0;



KeyboardState keyboard = {
    .up = 0,
    .down = 0,
    .left = 0,
    .right = 0,
    .currentLimb = 0,
};

void KeyboardUp(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
            keyboard.up = 0;
            break;
        case 's':
            keyboard.down = 0;
            break;
        case 'a': // left
            keyboard.left = 0;
            break;
        case 'd': // right
            keyboard.right = 0;
            break;
    }
}

void Keyboard(unsigned char key, int x, int y)
{
}



/*----------------------------------------------------------------*/



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

    /* For each object in the scene */
    // for (int i = 0; i < nbObjects; i++) {
    //
    //     GLint size;
    //     glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    //
    //     GLint RotationUniform = glGetUniformLocation(ShaderProgram, "ModelMatrix");
    //     if (RotationUniform == -1) {
    //         fprintf(stderr, "Could not bind uniform Model Matrix for cuboid %d.\n", i);
    //         exit(-1);
    //     }
    //     glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, cuboids[i].model);
    //
    //     /* Bind VAO of the current object */
    //     glBindVertexArray(cuboids[0].VAO);
    //     /* Draw the data contained in the VAO */
    //     glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, nullptr);
    //
    //     glBindVertexArray(defaultVAO);
    // }
    /* Swap between front and back buffer */
    glfwSwapBuffers(window);
}

/******************************************************************
 * updateBottom
 *******************************************************************/
void updateBottom(Cuboid *cuboid) {
    SetIdentityMatrix(cuboid->transformation);
    SetIdentityMatrix(cuboid->model);

    cuboid->current_rotation[1] = fmod(cuboid->current_rotation[1] + 0.0f, 360.0);
    SetRotationY(cuboid->current_rotation[1], RotationMatrixAnimY[cuboid->id]);

    SetIdentityMatrix(cuboid->transformation);
    MultiplyMatrix(RotationMatrixAnimY[cuboid->id], cuboid->transformation, cuboid->transformation);

    MultiplyMatrix(translations[cuboid->id], RotationMatrixAnim[cuboid->id], cuboid->model);
    MultiplyMatrix(cuboid->model, scales[cuboid->id], cuboid->model);
}

/******************************************************************
 * updateLimb
 * This function updates the limb's position, movement etc.
 * for every time unit
 *******************************************************************/

void updateLimb(Cuboid *cuboid, double delta) {

    float temp[16];
    SetIdentityMatrix(temp);
    SetIdentityMatrix(cuboid->transformation);
    // SetIdentityMatrix(cuboid->model);

    if (cuboid->parent_id != 0) {
        MultiplyMatrix(cuboid->transformation, cuboids[cuboid->parent_id].transformation, cuboid->transformation);
    }
    // 1. Initiate rotation around Y axis for all objects
    cuboid->current_rotation[1] = fmod(cuboid->current_rotation[1] + delta * 20.0f, 360.0);
    SetRotationY(cuboid->current_rotation[1], RotationMatrixAnimY[cuboid->id]);

    // Multiply all three rotation matrices
    MultiplyMatrix(RotationMatrixAnimX[cuboid->id], RotationMatrixAnimY[cuboid->id], RotationMatrixAnim[cuboid->id]);
    MultiplyMatrix(RotationMatrixAnim[cuboid->id], RotationMatrixAnimZ[cuboid->id], RotationMatrixAnim[cuboid->id]);

    MultiplyMatrix(RotationMatrixAnim[cuboid->id], cuboid->transformation, cuboid->transformation);

    // 2. Add Scaling to the transformation matrix
    // MultiplyMatrix(cuboid->transformation, RotationMatrixAnim[cuboid->id], cuboid->transformation);
    // MultiplyMatrix(cuboid->transformation, translations[cuboid->id],              cuboid->transformation);
    // MultiplyMatrix(translations[cuboid->id], RotationMatrixAnim[cuboid->id],       cuboid->transformation);
    MultiplyMatrix(cuboid->transformation, scales[cuboid->id],              cuboid->transformation);

    // 3. Add Translation to the transformation matrix
    MultiplyMatrix(RotationMatrixAnim[cuboid->id], translations[cuboid->id], cuboid->model);
    // MultiplyMatrix(cuboid->model, translations[cuboid->id], cuboid->model);
    // MultiplyMatrix(cuboid->model, RotationMatrixAnim[cuboid->id], cuboid->model);
    MultiplyMatrix(cuboid->model, scales[cuboid->id], cuboid->model);
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

    /* Create cube meshes */
    // for (int i = 0; i < nbObjects; i++) {
    //     GLuint *vbo = &cuboids[i].VBO;
    //     createCubeMesh(vbo, &cuboids[i].IBO, &cuboids[i].CBO, &cuboids[i].VAO);
    // }

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

    /* init scale matrix with a non-uniform scaling */
    for (int i = 0; i < nbObjects; i++) {
        /* Initialize scale matrices */
        SetScaleMatrix(cuboids[i].scale[0], cuboids[i].scale[1], cuboids[i].scale[2], scales[i]);

        /* Initialize Translation matrices */
        SetTranslation(cuboids[i].translation[0], cuboids[i].translation[1],
                       cuboids[i].translation[2], translations[i]);

        /* Initialize model matrices */
        SetIdentityMatrix(cuboids[i].model);
    }

    /* Initialize animation matrices */
    for (int i = 0; i < nbObjects; i++) {
        SetIdentityMatrix(RotationMatrixAnimX[i]);
        SetIdentityMatrix(RotationMatrixAnimY[i]);
        SetIdentityMatrix(RotationMatrixAnimZ[i]);
        SetIdentityMatrix(RotationMatrixAnim[i]);
    }


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

    cout << "current limb: " << keyboard.currentLimb << endl;

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

    /* Setup scene and rendering parameters */
    Initialize();

    Arm arm;
    arm.addLimb(THICKNESS, FIRST_LIMB_HEIGHT, THICKNESS);
    arm.addLimb(THICKNESS, SECOND_LIMB_LENGTH, THICKNESS);

    /* Rendering loop */
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        /* Update scene */
        arm.update(&keyboard);
        // OnIdle();
        /* Draw scene */
        Display(arm);
    }

    /* Close window */
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
