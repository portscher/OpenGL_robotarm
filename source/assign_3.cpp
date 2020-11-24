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

/* Number of objects in the scene */
#define nbObjects 4
/*----------------------------------------------------------------*/
/* Window parameters */
float winWidth = 1000.0f;
float winHeight = 800.0f;

/* window */
GLFWwindow *window;

/* Flag for starting/stopping animation */
GLboolean anim = GL_TRUE;

/* Indices to vertex attributes; in this case position only */
enum DataID {
    vPosition = 0, vColor = 1
};

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

    GLuint VBO;
    GLuint CBO;
    GLuint IBO;
    GLuint VAO;
} Cuboid;

GLuint defaultVAO;

Cuboid cuboids[nbObjects] = {
        // first (base)
        {.id = 1, .distance = 0, .transformation = {0}, .model = {0}, .scale{2.5f, 0.25f, 2.5f},
         .translation = {0.0,0.0,0.0}, .current_rotation = {0, 0, 0}},
        // second
        {.id = 2, .parent_id = 1, .distance = 0, .transformation = {0}, .model = {0}, .scale{0.3f, 2.5f, 0.3f},
         .translation = {0.0,2.25,.0}, .current_rotation = {0, 0, 0}},
        // third
        {.id = 3, .parent_id = 2, .distance = 0, .transformation = {0}, .model = {0}, .scale{1.5f, 0.3f, 0.3f},
         .translation = {1.125, 4.75,0}, .current_rotation = {0, 0, 0}},
         // fourth
        {.id = 4, .parent_id = 3, .distance = 2.5, .transformation = {0}, .model = {0}, .scale{0.3f, 1.0f, 0.3f},
         .translation = {2.35, 3.9,0}, .current_rotation = {0, 0, 0}},
};

/* Strings for loading and storing shader code */
static const char *VertexShaderString;
static const char *FragmentShaderString;

GLuint ShaderProgram;

/* Matrices for uniform variables in vertex shader */
float ProjectionMatrix[16];             /* Perspective projection matrix */
float ViewMatrix[16];                   /* Camera view matrix */
float ModelMatrix[nbObjects][16];       /* Array of model matrix */


/* Rotation matrices */
float RotationMatrixAnimX[16];
float RotationMatrixAnimY[16];
float RotationMatrixAnimZ[16];
float RotationMatrixAnim[16];

/* Reference time for animation */
double oldTime = 0;

/*----------------------------------------------------------------*/


/******************************************************************
*
* createCubeMesh
*
* This function creates a cube mesh and fills buffer objects with
* the geometry.
*
*******************************************************************/
void createCubeMesh(GLuint *VBO, GLuint *IBO, GLuint *CBO, GLuint *VAO) {
    GLfloat vertex_buffer_data[] = { /* 8 cube vertices XYZ */
            -1.0,   -1.0,   1.0,
            1.0,    -1.0,   1.0,
            1.0,    1.0,    1.0,
            -1.0,   1.0,    1.0,
            -1.0,   -1.0,   -1.0,
            1.0,    -1.0,   -1.0,
            1.0,    1.0,    -1.0,
            -1.0,   1.0,    -1.0,
    };

    GLfloat color_buffer_data[] = { /* RGB color values for 8 vertices */
            0.0, 0.0, 1.0,
            1.0, 0.0, 1.0,
            1.0, 1.0, 1.0,
            0.0, 1.0, 1.0,
            0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            1.0, 1.0, 0.0,
            0.0, 1.0, 0.0,
    };

    GLushort index_buffer_data[] = { /* Indices of 6*2 triangles (6 sides) */
            0, 1, 2,
            2, 3, 0,
            1, 5, 6,
            6, 2, 1,
            7, 6, 5,
            5, 4, 7,
            4, 0, 3,
            3, 7, 4,
            4, 5, 1,
            1, 0, 4,
            3, 2, 6,
            6, 7, 3,
    };

    /* Generate and write vertex buffer data */
    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

    /* Generate and write color buffer data */
    glGenBuffers(1, CBO);
    glBindBuffer(GL_ARRAY_BUFFER, *CBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

    /* Generate and write index buffer data */
    glGenBuffers(1, IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

    /* Generate vertex array object and fill it with VBO, CBO and IBO previously written*/
    glGenVertexArrays(1, VAO);
    glBindVertexArray(*VAO);
    /* Bind buffer with vertex data of currently active object */
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    /* Bind color buffer */
    glBindBuffer(GL_ARRAY_BUFFER, *CBO);
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    /* Bind index buffer */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *IBO);

    glBindVertexArray(defaultVAO);
}


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

void Display() {
    /* Clear window; color specified in 'Initialize()' */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* For each object in the scene */
    for (int i = 0; i < nbObjects; i++) {

        GLint size;
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

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

        GLint RotationUniform = glGetUniformLocation(ShaderProgram, "ModelMatrix");
        if (RotationUniform == -1) {
            fprintf(stderr, "Could not bind uniform ModelMatrix\n");
            exit(-1);
        }
        glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix[i]);


        /* Bind VAO of the current object */
        glBindVertexArray(cuboids[0].VAO);
        /* Draw the data contained in the VAO */
        glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, nullptr);

        glBindVertexArray(defaultVAO);
    }
    /* Swap between front and back buffer */
    glfwSwapBuffers(window);
}


/******************************************************************
 * updateLimb
 * This function updates the limb's position, movement etc.
 * for every time unit
 *******************************************************************/

void updateLimb(Cuboid* cuboid, double delta) {

    /* Increment rotation angle and update matrix */
    cuboid->current_rotation[1] = fmod(cuboid->current_rotation[1]  + delta * 20.0f, 360.0);
    SetRotationY(cuboid->current_rotation[1] , RotationMatrixAnimY);

    float temp[16];
    SetIdentityMatrix(temp);
    SetIdentityMatrix(cuboid->transformation);

    if (cuboid->id != 0) {
        SetTranslation(cuboid->distance, 0, 0, temp);
        MultiplyMatrix(cuboids[cuboid->parent_id].transformation, temp, cuboid->transformation);
    }

    MultiplyMatrix(RotationMatrixAnim, translations[cuboid->id-1], ModelMatrix[cuboid->id-1]);
    MultiplyMatrix(ModelMatrix[cuboid->id-1], scales[cuboid->id-1], ModelMatrix[cuboid->id-1]);
}


/******************************************************************
*
* OnIdle
*
* Function executed when no other events are processed; set by
* call to glutIdleFunc(); holds code for animation  
*
*******************************************************************/

void OnIdle() {
    /* Determine delta time between two frames to ensure constant animation */
    double newTime = glfwGetTime();
    double delta = newTime - oldTime;
    oldTime = newTime;

    /* Update of transformation matrices
     * Note order of transformations and rotation of reference axes */
    MultiplyMatrix(RotationMatrixAnimX, RotationMatrixAnimY, RotationMatrixAnim);
    MultiplyMatrix(RotationMatrixAnim, RotationMatrixAnimZ, RotationMatrixAnim);

    /* Apply scaling and translation to Model Matrices */
    for (int i = 0; i < nbObjects; i++) {
        updateLimb(&cuboids[i], anim ? delta : 0);
    }
}


/******************************************************************
*
* AddShader
*
* This function creates and adds individual shaders
*
*******************************************************************/

void AddShader(GLuint UsedShaderProgram, const char *ShaderCode, GLenum ShaderType) {
    /* Create shader object */
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    /* Associate shader source code string with shader object */
    glShaderSource(ShaderObj, 1, &ShaderCode, nullptr);

    GLint success = 0;
    GLchar InfoLog[1024];

    /* Compile shader source code */
    glCompileShader(ShaderObj);
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(ShaderObj, 1024, nullptr, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    /* Associate shader with shader program */
    glAttachShader(UsedShaderProgram, ShaderObj);
}


/******************************************************************
*
* CreateShaderProgram
*
* This function creates the shader program; vertex and fragment
* shaders are loaded and linked into program; final shader program
* is put into the rendering pipeline 
*
*******************************************************************/

void CreateShaderProgram() {
    /* Allocate shader object */
    ShaderProgram = glCreateProgram();

    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    /* Load shader code from file */
    VertexShaderString = LoadShader("../shaders/vertexshader.vs");
    FragmentShaderString = LoadShader("../shaders/fragmentshader.fs");

    /* Separately add vertex and fragment shader to program */
    AddShader(ShaderProgram, VertexShaderString, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, FragmentShaderString, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024];

    /* Link shader code into executable shader program */
    glLinkProgram(ShaderProgram);

    /* Check results of linking step */
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

    if (Success == 0) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), nullptr, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    /* Check if shader program can be executed */
    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);

    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), nullptr, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    /* Put linked shader program into drawing pipeline */
    glUseProgram(ShaderProgram);
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
    for (int i = 0; i < nbObjects; i++) {
        GLuint *vbo = &cuboids[i].VBO;
        createCubeMesh(vbo, &cuboids[i].IBO, &cuboids[i].CBO, &cuboids[i].VAO);
    }

    /* Set background (clear) color to blue */
    glClearColor(0.0, 0.0, 0.4, 0.0);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);


    /* Setup shaders and shader program */
    CreateShaderProgram();


    /* Initialize matrices */
    SetIdentityMatrix(ProjectionMatrix);
    SetIdentityMatrix(ViewMatrix);
    /* init scale matrix with a non-uniform scaling */


    for (int i = 0; i < nbObjects; i++) {
        /* Initialize scale matrices */
        SetScaleMatrix(cuboids[i].scale[0], cuboids[i].scale[1], cuboids[i].scale[2], scales[i]);

        /* Initialize Translation matrices */
        SetTranslation(cuboids[i].translation[0], cuboids[i].translation[1],
                       cuboids[i].translation[2], translations[i]);

        /* Initialize model matrices */
        SetIdentityMatrix(ModelMatrix[i]);
    }

    /* Initialize animation matrices */
    SetIdentityMatrix(RotationMatrixAnimX);
    SetIdentityMatrix(RotationMatrixAnimY);
    SetIdentityMatrix(RotationMatrixAnimZ);
    SetIdentityMatrix(RotationMatrixAnim);

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

    /* when key "Q" is pressed */
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        std::cout << "Q key pressed" << std::endl;
        /* activate/deactivate idle rotation */
        anim = !anim;
    }

    /* when key "W" is maintained pressed */
    if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
        std::cout << "W key repeat" << std::endl;
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

    /* Setup scene and rendering parameters */
    Initialize();

    /* Rendering loop */
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        /* Update scene */
        OnIdle();
        /* Draw scene */
        Display();
    }

    /* Close window */
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
