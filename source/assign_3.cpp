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

void UpdateCameraView();
void UpdateCameraPosition();
void LookAt(float*, float*, float*, float*);

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
        {.id = 0, .distance = 0, .transformation = {0}, .model = {0}, .scale{2.5f, 0.25f, 2.5f},
                .translation = {0}, .current_rotation = {0}},
        // first limb
        {.id = 1, .parent_id = 0, .distance = 0.2, .transformation = {0}, .model = {0}, .scale{THICKNESS,
                                                                                               FIRST_LIMB_HEIGHT,
                                                                                               THICKNESS},
                .translation = {0.0, FIRST_LIMB_Y_TRANSLATION, .0}, .current_rotation = {0}},
        // second limb
        {.id = 2, .parent_id = 1, .distance = 0.2, .transformation = {0}, .model = {0}, .scale{SECOND_LIMB_LENGTH,
                                                                                               THICKNESS, THICKNESS},
                .translation = {SECOND_LIMB_X_TRANSLATION, FIRST_LIMB_HEIGHT + FIRST_LIMB_Y_TRANSLATION,
                                0}, .current_rotation = {0}, .rotation_angle = 90},
        // third limb
        {.id = 3, .parent_id = 2, .distance = 0.2, .transformation = {0}, .model = {0}, .scale{THICKNESS,
                                                                                               THIRD_LIMB_LENGTH,
                                                                                               THICKNESS},
                .translation = {SECOND_LIMB_LENGTH + 0.9f,
                                FIRST_LIMB_HEIGHT + FIRST_LIMB_Y_TRANSLATION - THIRD_LIMB_LENGTH,
                                0}, .current_rotation = {0}},
};

/* Strings for loading and storing shader code */
static const char *VertexShaderString;
static const char *FragmentShaderString;

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


typedef struct keyboard {
    int up;
    int down;
    int left;
    int right;
} KeyboardState;

KeyboardState keyboard = {
    .up = 0,
    .down = 0,
    .left = 0,
    .right = 0,
};


// 0: camera, 1-3: limb
int currentLimb = 0;

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
    if (key == 'w')
    {
        keyboard.up = 1;
    }

    if (key == 's')
    {
        keyboard.down = 1;
    }

    if (key == 'a')
    {
        keyboard.left = 1;
    }

    if (key == 'd')
    {
        keyboard.right = 1;
    }
}

/*----------------------------------------------------------------*/

typedef struct camera {
    float currentPosition[3];
    float front[3];
    float up[3];
    float xAngle;
    float yAngle;
    int lookingAt;
    float fieldOfView;
    float viewMatrix[16];
    float projectionMatrix[16];
} Camera;

// Initialize camera
Camera cam = {
    .currentPosition = {0., 5., -25.},
    .front = {0., 0., -1.},
    .up = {0., 1., 0.,},
    .xAngle = -15.,
    .yAngle = 0.,
    .lookingAt = 0,
    .fieldOfView = 45.,
};

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
            -1.0, -1.0, 1.0,
            1.0, -1.0, 1.0,
            1.0, 1.0, 1.0,
            -1.0, 1.0, 1.0,
            -1.0, -1.0, -1.0,
            1.0, -1.0, -1.0,
            1.0, 1.0, -1.0,
            -1.0, 1.0, -1.0,
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
            fprintf(stderr, "Could not bind uniform Model Matrix for cuboid %d.\n", i);
            exit(-1);
        }
        glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, cuboids[i].model);

        /* Bind VAO of the current object */
        glBindVertexArray(cuboids[0].VAO);
        /* Draw the data contained in the VAO */
        glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, nullptr);

        glBindVertexArray(defaultVAO);
    }
    /* Swap between front and back buffer */
    glfwSwapBuffers(window);
}

void getCuboidPosition(Cuboid *cuboid, float angle, float result[3]) {
    result[0] = cuboid->distance * sinf(angle);
    result[1] = 0;
    result[2] = cuboid->distance * cosf(angle);

    // for cuboids with an inclined "orbit":
    if (cuboid->rotation_angle != 0) {
        result[0] = (result[0] * cosf(cuboid->rotation_angle)) - (result[1] * sinf(cuboid->rotation_angle));
        result[1] = (result[0] * sinf(cuboid->rotation_angle)) + (result[1] * cosf(cuboid->rotation_angle));
        result[2] = result[2];
    }
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

    SetIdentityMatrix(cuboid->transformation);

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
    MultiplyMatrix(cuboid->transformation, scales[cuboid->id], cuboid->transformation);

    // 3. Add Translation to the transformation matrix
    MultiplyMatrix(RotationMatrixAnim[cuboid->id], translations[cuboid->id], cuboid->model);
    MultiplyMatrix(cuboid->model, scales[cuboid->id], cuboid->model);
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

    /* Apply scaling and translation to Model Matrices */
    updateBottom(&cuboids[0]);
    for (int i = 1; i < nbObjects; i++) {
        // if (currentLimb == i) {
            updateLimb(&cuboids[i], anim ? delta : 0);
        // }
    }

    UpdateCameraView();
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
        SetIdentityMatrix(cuboids[i].model);
    }

    /* Initialize animation matrices */
    for (int i = 0; i < nbObjects; i++) {
        SetIdentityMatrix(RotationMatrixAnimX[i]);
        SetIdentityMatrix(RotationMatrixAnimY[i]);
        SetIdentityMatrix(RotationMatrixAnimZ[i]);
        SetIdentityMatrix(RotationMatrixAnim[i]);
    }


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

    switch(key)
    {
        case GLFW_KEY_W: // forwards
            keyboard.up = action == GLFW_PRESS;
            break;
        case GLFW_KEY_S: // backwards
            keyboard.down = action == GLFW_PRESS;
            break;
        case GLFW_KEY_A: // left
            keyboard.left = action == GLFW_PRESS;
            break;
        case GLFW_KEY_D: // right
            keyboard.right = action == GLFW_PRESS;
            break;
        case GLFW_KEY_1: case GLFW_KEY_2: case GLFW_KEY_3: case GLFW_KEY_0:
            currentLimb = key - 48; // GLFW_KEY_0 = 48
            break;
        case GLFW_KEY_Q:
            if (action == GLFW_PRESS) {
                std::cout << "Bye!" << std::endl;
                exit(0);
            }
    }

    std::cout << "current limb: " << currentLimb << std::endl;

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
