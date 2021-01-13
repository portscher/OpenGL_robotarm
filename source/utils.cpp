#include "Vector.hpp"
#include "utils.hpp"
#include "OBJParser.hpp"            /* Loading function for triangle meshes in OBJ format */
#include "LoadTexture.hpp"

/******************************************************************
*
* @brief This function read the content of an OBJ file and then fill the
* buffer objects with the data
*
* @param filename = name of file.obj
* @param scale = scale factor applied to the vertices
* @param rgb = 3D vector containing the color of the object (r=x, g=y, b=z)
*******************************************************************/
void readMeshFile(string filename, float scale, Vector rgb,
        GLuint *CBO, GLuint *NBO, GLuint *VAO)
{
    GLuint VBO;
    GLuint IBO;
    GLuint UVBO;

    /* Structure for loading of OBJ data */
    obj_scene_data data;

    /* Load first OBJ model */
    int success = parse_obj_scene(&data, filename.c_str());

    if(!success)
        printf("Could not load file. Exiting.\n");

    /*  Copy mesh data from structs into appropriate arrays */
    int indx = data.face_count;

    GLushort* index_buffer_data = (GLushort*) calloc (indx*3, sizeof(GLushort));
    GLfloat* vertex_buffer_data = (GLfloat*) calloc (indx*9, sizeof(GLfloat));
    GLfloat* color_buffer_data = (GLfloat*) calloc (indx*9, sizeof(GLfloat));
    GLfloat* normal_buffer_data = (GLfloat*) calloc (indx*9, sizeof(GLfloat));
    GLfloat* uv_buffer_data = (GLfloat*) calloc (indx*6, sizeof(GLfloat));

    /* for each triangle... */
    for(int i=0; i<indx; i++)
    {
        int offset3D = i*9;
        int offset2D = i*6;

        /* fill VBO for this triangle (x,y,z coords for 3 vertices = 9 values) */
        for(int j=0; j<3; j++)
        {
            /* Index of current vertex */
            int idVert = (GLushort)(*data.face_list[i]).vertex_index[j];
            vertex_buffer_data[offset3D + j*3 ] = (GLfloat)(*data.vertex_list[idVert]).e[0]*scale;
            vertex_buffer_data[offset3D + j*3 + 1] = (GLfloat)(*data.vertex_list[idVert]).e[1]*scale;
            vertex_buffer_data[offset3D + j*3 + 2] = (GLfloat)(*data.vertex_list[idVert]).e[2]*scale;
        }

        /* fill Normal buffer for this triangle */
        if( (*data.face_list[i]).normal_index[0] != -1 )
        {
            for(int j=0; j<3; j++)
            {
                int idNorm = (GLushort)(*data.face_list[i]).normal_index[j];
                normal_buffer_data[offset3D + j*3 ] = (GLfloat)(*data.vertex_normal_list[idNorm]).e[0];
                normal_buffer_data[offset3D + j*3 + 1] = (GLfloat)(*data.vertex_normal_list[idNorm]).e[1];
                normal_buffer_data[offset3D + j*3 + 2] = (GLfloat)(*data.vertex_normal_list[idNorm]).e[2];
            }
        }
        else
        {
            for(int j=0; j<3; j++)
            {
                normal_buffer_data[offset3D + j*3 ] = vertex_buffer_data[offset3D + j*3 ];
                normal_buffer_data[offset3D + j*3 + 1] = vertex_buffer_data[offset3D + j*3 + 1];
                normal_buffer_data[offset3D + j*3 + 2] = vertex_buffer_data[offset3D + j*3 + 2];
            }
        }

        /* fill UV buffer for this triangle */
        if( (*data.face_list[i]).texture_index[0] != -1 )
        {
            for(int j=0; j<3; j++)
            {
                int idUV = (GLushort)(*data.face_list[i]).texture_index[j];
                uv_buffer_data[offset2D + j*2 ] = (GLfloat)(*data.vertex_texture_list[idUV]).e[0];
                uv_buffer_data[offset2D + j*2 + 1] = (GLfloat)(*data.vertex_texture_list[idUV]).e[1];
            }
        }

        /* fill Color buffer for this triangle */
        for(int j=0; j<3; j++)
        {
            color_buffer_data[offset3D + j*3 ] = (GLfloat)(rgb.x);
            color_buffer_data[offset3D + j*3 + 1] = (GLfloat)(rgb.y);
            color_buffer_data[offset3D + j*3 + 2] = (GLfloat)(rgb.z);
        }

        /* Fill indices buffer for this triangles (3 indices) */
        index_buffer_data[i*3] = i*3;
        index_buffer_data[i*3+1] = i*3+1;
        index_buffer_data[i*3+2] = i*3+2;
    }


    /* Create buffer objects and load data into buffers*/
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.face_count*9*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, CBO);
    glBindBuffer(GL_ARRAY_BUFFER, *CBO);
    glBufferData(GL_ARRAY_BUFFER, data.face_count*9*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, NBO);
    glBindBuffer(GL_ARRAY_BUFFER, *NBO);
    glBufferData(GL_ARRAY_BUFFER, data.face_count*9*sizeof(GLfloat), normal_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &UVBO);
    glBindBuffer(GL_ARRAY_BUFFER, UVBO);
    glBufferData(GL_ARRAY_BUFFER, data.face_count*6*sizeof(GLfloat), uv_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.face_count*3*sizeof(GLushort), index_buffer_data, GL_STATIC_DRAW);


    /* Generate vertex array object and fill it with VBO, CBO and IBO previously written*/
    glGenVertexArrays(1, VAO);
    glBindVertexArray(*VAO);

    /* Bind buffer with vertex data of currently active object */
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    /* Bind color buffer */
    glBindBuffer(GL_ARRAY_BUFFER, *CBO);
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, nullptr);

    /* Bind normal buffer */
    glEnableVertexAttribArray(vNormal);
    glBindBuffer(GL_ARRAY_BUFFER, *NBO);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT,GL_FALSE, 0, nullptr);

    /* Bind uv buffer */
    glEnableVertexAttribArray(vUV);
    glBindBuffer(GL_ARRAY_BUFFER, UVBO);
    glVertexAttribPointer(vUV, 2, GL_FLOAT,GL_FALSE, 0, nullptr);

    /* Bind index buffer */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glBindVertexArray(0);
}

/******************************************************************
*
* SetupTexture
*
* This function is called to load the texture and initialize
* texturing parameters
*
* Input: TextureID = id of the texture to setup
*        filename = path to bitmap file to read
*******************************************************************/

void SetupTexture(GLuint *TextureID, const char* filename)
{
    /* Allocate texture container */
    TextureDataPtr* Texture = (TextureDataPtr*)malloc(sizeof(TextureDataPtr));

    int success = LoadTexture(filename, Texture);
    if (!success)
    {
        printf("Error loading texture. Exiting.\n");
        exit(-1);
    }

    /* Create texture name and store in handle */
    glGenTextures(1, TextureID);

    /* Bind texture */
    glBindTexture(GL_TEXTURE_2D, *TextureID);

    /* Load texture image into memory */
    glTexImage2D(GL_TEXTURE_2D,     /* Target texture */
                 0,                 /* Base level */
                 GL_RGB,            /* Each element is RGB triple */
                 Texture->width,    /* Texture dimensions */
                 Texture->height,
                 0,                 /* Border should be zero */
                 GL_BGR,            /* Data storage format for BMP file */
                 GL_UNSIGNED_BYTE,  /* Type of pixel data, one byte per channel */
                 Texture->data);    /* Pointer to image data  */

    /* Next set up texturing parameters */

    /* Repeat texture on edges when tiling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    /* Linear interpolation for magnification */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Trilinear MIP mapping for minification */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    /* Note: MIP mapping not visible due to fixed, i.e. static camera */

}


/******************************************************************
*
* @brief This function creates a cube mesh and fills buffer objects with
* the geometry.
*
* @param width of the object
* @param height of the object
* @param colour of the object, as RGB values
*
*******************************************************************/
GLuint createCubeMesh(float width, float height, float *colour)
{
    GLuint VAO;
    GLuint VBO;
    GLuint IBO;
    GLuint CBO;

    float depth = width;
    GLfloat vertex_buffer_data[] = { /* 8 cube vertices XYZ */
            0., 0., depth,
            width, 0., depth,
            width, height, depth,
            0., height, depth,

            0., 0., 0.,
            width, 0., 0.,
            width, height, 0.,
            0., height, 0.,
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
* @brief This function creates and adds individual shaders
*
*******************************************************************/
void AddShader(GLuint UsedShaderProgram, const char *ShaderCode, GLenum ShaderType)
{
    /* Create shader object */
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0)
    {
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

    if (!success)
    {
        glGetShaderInfoLog(ShaderObj, 1024, nullptr, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    /* Associate shader with shader program */
    glAttachShader(UsedShaderProgram, ShaderObj);
}

/******************************************************************
*
* @brief This function creates the shader program; vertex and fragment
* shaders are loaded and linked into program; final shader program
* is put into the rendering pipeline
*
*******************************************************************/
GLuint CreateShaderProgram(string vsPath, string fsPath)
{
    GLuint ShaderProgram = glCreateProgram();
    if (ShaderProgram == 0)
    {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    /* Strings for loading and storing shader code */
    const char *VertexShaderString;
    const char *FragmentShaderString;

    /* Load shader code from file */
    VertexShaderString = LoadShader(vsPath.c_str());
    FragmentShaderString = LoadShader(fsPath.c_str());

    /* Separately add vertex and fragment shader to program */
    AddShader(ShaderProgram, VertexShaderString, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, FragmentShaderString, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024];

    /* Link shader code into executable shader program */
    glLinkProgram(ShaderProgram);

    /* Check results of linking step */
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

    if (Success == 0)
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), nullptr, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    /* Check if shader program can be executed */
    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);

    if (!Success)
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), nullptr, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    return ShaderProgram;
}


/**
 * @brief Constrains an angle to a range from 0 to 359 degrees.
 *
 * @param x = the angle to be constrained
 *
 */
float constrainAngle(float x)
{
    x = fmodf(x, 360);
    if (x < 0)
        x += 360;
    return x;
}

/* A value is saved in a buffer at the GPU */
void BindUniform1f(const string name, GLuint program, float val)
{
    GLint uniform = glGetUniformLocation(program, name.c_str());
    glUniform1f(uniform, val);
}

void BindUniform4f(const string name, GLuint program, float* val)
{
    GLint uniform = glGetUniformLocation(program, name.c_str());
    glUniformMatrix4fv(uniform, 1, GL_TRUE, val);
}


void BindUniform3f(const string name, GLuint program, Vector val)
{
    GLint uniform = glGetUniformLocation(program, name.c_str());
    glUniform3f(uniform, val.x, val.y, val.z);
}

/*
 *  VBO vertex buffer object
 *  CBO color buffe object
 *  NBO normal buffer object
 *  IBO index buffer object
 *  UVBO uv buffer object
 */
int BindBasics(GLuint VBO, GLuint CBO, GLuint IBO, GLuint NBO, GLuint UVBO)
{
    /* Bind buffer with vertex data of currently active object */
    glEnableVertexAttribArray(vPosition);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    /* Bind color buffer */
    glEnableVertexAttribArray(vColor);
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);

    /* Bind normal buffer */
    if (NBO != 0) {
        glEnableVertexAttribArray(vNormal);
        glBindBuffer(GL_ARRAY_BUFFER, NBO);
        glVertexAttribPointer(vNormal, 3, GL_FLOAT,GL_FALSE, 0, 0);
    }

    /* Bind uv buffer */
    if (UVBO != 0) {
        glEnableVertexAttribArray(vUV);
        glBindBuffer(GL_ARRAY_BUFFER, UVBO);
        glVertexAttribPointer(vUV, 2, GL_FLOAT,GL_FALSE, 0, 0);
    }

    /* Bind index buffer */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    GLint size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

    return size/sizeof(unsigned int);
}


