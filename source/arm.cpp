#include "arm.hpp"

Arm::Arm() :
    internal{0}, height(.25), width(2.5) {
    // base
    VAO = createCubeMesh(width, height);
    SetIdentityMatrix(internal);
}

void Arm::addLimb(float w, float h) {
    int currentIndex = limbs.size() - 1;

    // calculate offset to floor
    float offset = 0;
    for (auto l : limbs) {
        offset += l->offset(); // HEHERE
    }

    float center = 0;
    if (limbs.size() == 0) {
        center = width/2;
        offset += height;
    }

    cout << "creating limb: " <<
        "(x, y, z): " << center << ", " << offset << ", 0" <<
        "(w, h)" << w << ", " << h << endl;

    float pos[] = {center, offset, center};
    float size[] = {w, h};
    limbs.push_back(new Limb(currentIndex, pos, size));
}
void Arm::update(KeyboardState *state) {
    for (int i = 0; i != limbs.size(); i++) {
        float transformation[16];
        SetIdentityMatrix(transformation);

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

        if (i > 0) { // update only children of the first limb
            limbs.at(i-1)->getTransformation(transformation);
        }

        limbs.at(i)->update(transformation);
    }
}

void Arm::display(GLint ShaderProgram) {
    GLint size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

    // TODO this is actually the transformation
    GLint ModelUniform = glGetUniformLocation(ShaderProgram, "ModelMatrix");
    if (ModelUniform == -1) {
        fprintf(stderr, "Could not bind uniform Model Matrix for cuboid.\n");
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
