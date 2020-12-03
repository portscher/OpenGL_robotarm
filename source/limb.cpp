#include "limb.hpp"

/******************************************************************
*
* Constructs a limb using the
*
*******************************************************************/
Limb::Limb(int _ID, const float _position[3], float size[2], float colour[3]) :
        rotationX(0), rotationY(0), rotationZ(0),
        position{0}, length(size[1]), width(size[0]),
        internal{0}, transformation{0}, model{0}
{

    ID = _ID;
    VAO = createCubeMesh(width, length, colour);

    SetIdentityMatrix(internal);
    SetIdentityMatrix(transformation);
    SetIdentityMatrix(model);
    // WTH? anyways.. it works
    position[0] = _position[0];
    position[1] = _position[1];
    position[2] = _position[2];
}

float Limb::offset()
{
    return length;
}

void Limb::setRotation(int axis, float deg)
{
    cout << "updating rotation axis " << axis << " to " << (deg < 360 ? deg : deg - 360) << endl;
    switch (axis)
    {
        case 0:
            rotationX = deg;
            break;
        case 1:
            rotationY = deg;
            break;
        case 2:
            rotationZ = deg;
            break;
        default:
            cout << "setRotation on " << ID << " can't be set, " << axis << " doesn't exist. " << endl;
    }
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
            cout << "getRotation on " << ID << " cant return a value" << endl;
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

void Limb::display(GLint ShaderProgram)
{
    GLint size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

    // TODO this is actually the transformation
    GLint ModelUniform = glGetUniformLocation(ShaderProgram, "ModelMatrix");
    if (ModelUniform == -1)
    {
        fprintf(stderr, "Could not bind uniform Model Matrix for cuboid %d.\n", ID);
        exit(-1);
    }
    glUniformMatrix4fv(ModelUniform, 1, GL_TRUE, model);

    /* Bind VAO of the current object */
    glBindVertexArray(VAO);
    /* Draw the data contained in the VAO */
    glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, nullptr);

    glBindVertexArray(0);
}
