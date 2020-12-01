#include "utils.hpp"

/******************************************************************
*
* createCubeMesh
*
* This function creates a cube mesh and fills buffer objects with
* the geometry.
*
*******************************************************************/
GLuint createCubeMesh(float width, float height, float* colour) {
    GLuint VAO;
    GLuint VBO;
    GLuint IBO;
    GLuint CBO;

    float depth = width;
    GLfloat vertex_buffer_data[] = { /* 8 cube vertices XYZ */
            0.,     0.,     depth,
            width,  0.,     depth,
            width,  height, depth,
            0.,     height, depth,

            0.,     0.,     0.,
            width,  0.,     0.,
            width,  height, 0.,
            0.,     height, 0.,
    };

    GLfloat color_buffer_data[] = { /* RGB color values for 8 vertices */
            colour[0], colour[1], colour[2],
            colour[0], colour[1], colour[2],
            colour[0], colour[1], colour[2],
            colour[0], colour[1], colour[2],
            colour[0], colour[1], colour[2],
            colour[0], colour[1], colour[2],
            colour[0], colour[1], colour[2],
            colour[0], colour[1], colour[2]
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
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

    /* Generate and write color buffer data */
    glGenBuffers(1, &CBO);
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

    /* Generate and write index buffer data */
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

    /* Generate vertex array object and fill it with VBO, CBO and IBO previously written*/
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    /* Bind buffer with vertex data of currently active object */
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    /* Bind color buffer */
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    /* Bind index buffer */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glBindVertexArray(0);

    return VAO;
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
void CreateShaderProgram(GLuint ShaderProgram) {
    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    /* Strings for loading and storing shader code */
    const char *VertexShaderString;
    const char *FragmentShaderString;

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

void printMatrix(float* mat)
{
    int i;
    for(i = 0; i < 16; i++) {
        if (fmod(i, 4) == 0) {
            printf("\n");
        }
        printf("%g ", mat[i]);
    }
}
