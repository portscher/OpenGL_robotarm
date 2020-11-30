#include "arm.hpp"

Arm::Arm() :
    internal{0}, height(.25), width(2.5) {
    // base
    VAO = createCubeMesh();
    SetIdentityMatrix(internal);

    // internal tranformations
    SetScaleMatrix(width, height, width, internal);
}

void Arm::addLimb(float width, float height, float depth) {
    int currentIndex = limbs.size() - 1;

    // calculate offset to floor
    float offset = height;
    for (auto l : limbs) {
        offset += l->offset();
    }

    cout << "creating limb: " <<
        "(x, y, z): " << width/2 << ", " << offset << ", 0" <<
        "(w, h)" << width << ", " << height << endl;
    float pos[] = {width/2, offset, 0.};
    float size[] = {width, height, depth};
    limbs.push_back(new Limb(currentIndex, pos, size));
}

void Arm::update(KeyboardState *state) {
    for (int i = 0; i != limbs.size(); i++) {
        if (state->currentLimb != 0 && state->currentLimb-1 == i) {
            if (state->up) {
                float rot = limbs.at(i)->getRotation(0);
                limbs.at(i)->setRotation(0, ++rot);
            } else if (state->down) {
                float rot = limbs.at(i)->getRotation(0);
                limbs.at(i)->setRotation(0, --rot);
            } else if (state->left) {
                float rot = limbs.at(i)->getRotation(1);
                limbs.at(i)->setRotation(1, ++rot);
            } else if (state->right) {
                float rot = limbs.at(i)->getRotation(1);
                limbs.at(i)->setRotation(1, --rot);
            }
        }
        limbs.at(i)->update();
    }
}

void Arm::display(GLint ShaderProgram) {
    GLint size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

    // TODO this is actually the transformation
    GLint ModelUniform = glGetUniformLocation(ShaderProgram, "ModelMatrix");
    if (ModelUniform == -1) {
        fprintf(stderr, "Could not bind uniform Model Matrix for cuboid %d.\n");
        exit(-1);
    }
    glUniformMatrix4fv(ModelUniform, 1, GL_TRUE, internal);

    /* Bind VAO of the current object */
    glBindVertexArray(VAO);
    /* Draw the data contained in the VAO */
    glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, nullptr);

    for (auto limb : limbs) {
        limb->display(ShaderProgram);
    }
}

Limb Arm::getLimb(int index) {
    // TODO return pointer
    return *limbs.at(index);
}
