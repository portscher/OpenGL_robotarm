#include "arm.hpp"
#include "Vector.hpp"

/******************************************************************
*
* @brief Constructs a new Arm object (with static base)
*
*******************************************************************/
Arm::Arm() :
        internal{0}
{
    // base
    Vector baseColour = {0.9f, 0.9f, 0.5f};
    VAO = readMeshFile("../models/smp-base.obj", 1.5f, baseColour);
    SetIdentityMatrix(internal);
}

/******************************************************************
*
* @brief adds a new limb to the arm
*
* @param w = width
* @param h = height
*******************************************************************/
void Arm::addLimb(std::string filename, float offset, Vector colour, float scale)
{
    int currentIndex = limbs.size() - 1;

    float center = 0;

    std::cout << "creating limb: " <<
         "(x, y, z): " << center << ", " << offset << ", 0. " << std::endl;

    float pos[] = {center, offset, center};

    limbs.push_back(new Limb(currentIndex, filename, pos, colour, scale));
}

/******************************************************************
*
* @brief updates every limb according to the keyboard input
*
* @param state = a reference to the keyboard state
* (up, down, left, right)
*
*******************************************************************/
void Arm::update(KeyboardState *state)
{
    // reset the arm to its initial position (all straight)
    if (state->reset)
    {
        for (auto &limb : limbs)
        {
            for (int k = 0; k < 3; k++)
            {
                limb->setRotation(k, 0);
            }
        }
    }

    for (int i = 0; i != limbs.size(); i++)
    {
        float transformation[16];
        float rot;
        SetIdentityMatrix(transformation);

        // rotate along the axis chosen via keyboard
        if (state->currentLimb != 0 && state->currentLimb - 1 == i)
        {
            if (state->up)
            {
                rot = getCurrentRotationAt(0, limbs.at(i));
                limbs.at(i)->setRotation(0, ++rot);
            } else if (state->down)
            {
                rot = getCurrentRotationAt(0, limbs.at(i));
                limbs.at(i)->setRotation(0, --rot);
            } else if (state->left)
            {
                rot = getCurrentRotationAt(1, limbs.at(i));
                limbs.at(i)->setRotation(1, ++rot);
            } else if (state->right)
            {
                rot = getCurrentRotationAt(1, limbs.at(i));
                limbs.at(i)->setRotation(1, --rot);
            }
        }

        if (i > 0)
        { // update only children of the first limb
            limbs.at(i - 1)->getTransformation(transformation);
        }

        limbs.at(i)->update(transformation);
    }
}

/******************************************************************
*
* @brief ets the current rotation of a limb and returns its angle,
* normalized to a range from 0 to 359 degrees
*
* @param axis = around which the limb is rotating
* @param limb = a reference to the limb
*******************************************************************/
float Arm::getCurrentRotationAt(int axis, Limb *limb)
{
    float temp = limb->getRotation(axis);
    return constrainAngle(temp);
}

void Arm::display(GLint ShaderProgram)
{
    GLint size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

    GLint ModelUniform = glGetUniformLocation(ShaderProgram, "ModelMatrix");
    if (ModelUniform == -1)
    {
        fprintf(stderr, "Could not bind uniform Model Matrix for cuboid.\n");
        exit(-1);
    }
    glUniformMatrix4fv(ModelUniform, 1, GL_TRUE, internal);

    /* Bind VAO of the current object */
    glBindVertexArray(VAO);
    /* Draw the data contained in the VAO */
    glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, nullptr);

    for (auto limb : limbs)
    {
        limb->display(ShaderProgram);
    }
}
