#include "limb.hpp"
#include "utils.hpp"
#include "Vector.hpp"

/******************************************************************
*
* Constructs a limb using the
*
*******************************************************************/
Limb::Limb(Arm *_arm, int _ID, std::string filename, float _position[3], Vector colour, float scale) :
        arm(_arm), rotationX(0), rotationY(0), rotationZ(0),
        position{_position[0], _position[1], _position[2]},
        internal{0}, transformation{0}, model{0}
{
    ID = _ID;

    readMeshFile(filename, scale, colour, &CBO, &NBO, &VAO);
    SetIdentityMatrix(internal);
    SetRotationZ(270, internal);
    SetIdentityMatrix(transformation);
    SetIdentityMatrix(model);
}

void Limb::setRotation(int axis, float deg)
{
    std::string axis_name;
    switch (axis)
    {
        case 0:
            rotationX = deg;
            axis_name = "x";
            break;
        case 1:
            rotationY = deg;
            axis_name = "y";
            break;
        case 2:
            rotationZ = deg;
            axis_name = "z";
            break;
        default:
            std::cout << "setRotation on " << ID << " can't be set, " << axis << " doesn't exist. " << std::endl;
    }
    std::cout << "updating rotation on " << axis_name << " axis to " << (deg < 360 ? deg : deg - 360) << " degrees" << std::endl;
}

/** Returns the rotation angle around a given axis */
float Limb::getRotation(int axis)
{
    switch (axis)
    {
        case 0:
            return rotationX;
        case 1:
            return rotationY;
        case 2:
            return rotationZ;
        default:
            std::cout << "getRotation on " << ID << " cant return a value" << std::endl;
            return 0;
    }
}

void Limb::setAngle(int deg)
{
    angle = deg;
}

/** Returns the transformation matrix of a limb */
void Limb::getTransformation(float *result)
{
    memcpy(result, transformation, 16 * sizeof(float));
}

/******************************************************************
*
* update - updates a limb, taking into account the transformation
* of the parent limb
*
* parentTransform = transformation matrix of the parent limb
*******************************************************************/
void Limb::update(float *parentTransform)
{
    // reset transformations
    SetIdentityMatrix(model);
    SetIdentityMatrix(transformation);

    float result[16];
    SetIdentityMatrix(result);

    // move to base
    float pos[16];
    SetTranslation(position[0], position[1], position[2], pos);

    float rot[16];
    SetRotationY(rotationY, rot);
    MultiplyMatrix(result, rot, result);

    SetRotationX(rotationX, rot);
    MultiplyMatrix(result, rot, result);

    SetRotationZ(rotationZ, rot);
    MultiplyMatrix(result, rot, result);

    MultiplyMatrix(result, rot, result);

    MultiplyMatrix(result, model, model);
    MultiplyMatrix(pos, model, model);
    MultiplyMatrix(parentTransform, model, model);
    MultiplyMatrix(transformation, model, transformation);
}

void Limb::display(GLint program)
{
    GLint size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

    GLint ModelUniform = glGetUniformLocation(program, "ModelMatrix");
    if (ModelUniform == -1)
    {
        fprintf(stderr, "Could not bind uniform Model Matrix for cuboid %d.\n", ID);
        exit(-1);
    }
    glUniformMatrix4fv(ModelUniform, 1, GL_TRUE, model);

    glEnableVertexAttribArray(vColor);
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(vNormal);
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(VAO);
    /* Draw the data contained in the VAO */
    glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, nullptr);

    glBindVertexArray(0);
}
