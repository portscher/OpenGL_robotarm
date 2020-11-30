#include "limb.hpp"

Limb::Limb(int _ID, float position[3], float size[3]) :
    internal{0}, transformation{0}, model{0},
    rotationX(0), rotationY(0), rotationZ(0)
{
    ID = _ID;
    VAO = createCubeMesh();

    // SetIdentityMatrix(internal);
    // SetIdentityMatrix(transformation);
    SetIdentityMatrix(model);

    SetScaleMatrix(size[0], size[1], size[2], internal);
}

float Limb::offset() {
    return _offset;
}

// TODO axis could be an enum
void Limb::setRotation(int axis, float grad) {
    cout << "updating rotation axis " << axis << " to " << grad << endl;
    switch (axis) {
        case 0:
            rotationX = grad;
            break;
        case 1:
            rotationY = grad;
            break;
        case 2:
            rotationZ = grad;
            break;
    }
}

float Limb::getRotation(int axis) {
    switch (axis) {
        case 0:
            return rotationX;
        case 1:
            return rotationY;
        case 2:
            return rotationZ;
        default:
            // TODO throw exception?
            cout << "getRotation on " << ID << " cant return a value" << endl;
            return 0;
    }
}

void Limb::setAngle(int grad) {
    angle = grad;
}

void Limb::getTransformation(float *result) {
    memcpy(result, transformation, 16*sizeof(float));
}

void Limb::update(float *parentTransform) {
    // // SetIdentityMatrix(transformation);
    // SetIdentityMatrix(model);
    //

    SetTranslation(position[0], position[1], position[2], transformation);
    // // 1. Initiate rotation around Y axis for all objects
    float rot[16];
    SetRotationX(rotationX, rot);
    MultiplyMatrix(transformation, rot, transformation);

    SetRotationY(rotationY, rot);
    MultiplyMatrix(transformation, rot, transformation);

    SetRotationZ(rotationZ, rot);
    MultiplyMatrix(transformation, rot, transformation);

    // Multiply all three rotation matrices
    // MultiplyMatrix(RotationMatrixAnimX[cuboid->id], RotationMatrixAnimY[cuboid->id], RotationMatrixAnim[cuboid->id]);
    // MultiplyMatrix(RotationMatrixAnim[cuboid->id], RotationMatrixAnimZ[cuboid->id], RotationMatrixAnim[cuboid->id]);

    // MultiplyMatrix(RotationMatrixAnim[cuboid->id], cuboid->transformation, cuboid->transformation);
    //
    // // 2. Add Scaling to the transformation matrix
    // // MultiplyMatrix(cuboid->transformation, RotationMatrixAnim[cuboid->id], cuboid->transformation);
    // // MultiplyMatrix(cuboid->transformation, translations[cuboid->id],              cuboid->transformation);
    // // MultiplyMatrix(translations[cuboid->id], RotationMatrixAnim[cuboid->id],       cuboid->transformation);
    // MultiplyMatrix(cuboid->transformation, scales[cuboid->id],              cuboid->transformation);
    //
    // // 3. Add Translation to the transformation matrix
    // MultiplyMatrix(RotationMatrixAnim[cuboid->id], translations[cuboid->id], cuboid->model);
    // // MultiplyMatrix(cuboid->model, translations[cuboid->id], cuboid->model);
    // // MultiplyMatrix(cuboid->model, RotationMatrixAnim[cuboid->id], cuboid->model);
    // MultiplyMatrix(transformation, internal, model);

    // MultiplyMatrix(RotationMatrixAnim[cuboid->id], translations[cuboid->id], cuboid->model);
    // MultiplyMatrix(model, internal, model);
    // MultiplyMatrix(model, transformation, model);
    MultiplyMatrix(parentTransform, transformation, transformation);

    MultiplyMatrix(transformation, internal, model);

}

void Limb::display(GLint ShaderProgram) {
    GLint size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

    // TODO this is actually the transformation
    GLint ModelUniform = glGetUniformLocation(ShaderProgram, "ModelMatrix");
    if (ModelUniform == -1) {
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
