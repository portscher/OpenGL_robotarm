#include "camera.hpp"

Camera::Camera() :
    currentPosition{ 0.0, 5.0, -25.0 },
    front{ 0.0, 0.0, 1.0 },
    up{ 0.0, 1.0, 0.0 },
    xAngle(-15.0),
    yAngle(0.0),
    lookingAt(0.0),
    fieldOfView(45.0)
{
}

void Camera::UpdateView()
{
    float target[3];
    this->front[0] = cosf(ToRadian(this->xAngle)) * sinf(ToRadian(this->yAngle));
    this->front[1] = sinf(ToRadian(this->xAngle));
    this->front[2] = cosf(ToRadian(this->xAngle)) * sinf(ToRadian(this->yAngle));
    NormalizeVector(this->front, 3, this->front);
    Add(this->currentPosition, this->front, 3, target);

    printf("pos: [%g,%g, %g]\nfront: [%g, %g, %g]\nlookingat: [%g, %g, %g]\n",
    this->currentPosition[0], this->currentPosition[1], this->currentPosition[2],
    this->front[0], this->front[1], this->front[2],
    target[0], target[1], target[2]);

    this->LookAt(this->currentPosition, target, this->up, this->viewMatrix);

    float aspect = winWidth / winHeight;
    float nearPlane = 1.0;
    float farPlane = 50.0;
    SetPerspectiveMatrix(this->fieldOfView, aspect, nearPlane, farPlane, this->projectionMatrix);
}

void Camera::MoveUp(float speed)
{
    float temp[3];
    ScalarMultiplication(speed, this->front, 3, temp);
    Add(this->currentPosition, temp, 3, this->currentPosition);
}

void Camera::MoveDown(float speed)
{
    float temp[3];
    ScalarMultiplication(speed, this->front, 3, temp);
    Substract(this->currentPosition, temp, 3, this->currentPosition);
} 

void Camera::MoveLeft(float speed)
{
    float temp[3];
    CrossProduct(this->front, this->up, temp);
    NormalizeVector(temp, 3, temp);
    ScalarMultiplication(speed * 0.8, temp, 3, temp);
    Substract(this->currentPosition, temp, 3, this->currentPosition);
}

void Camera::MoveRight(float speed)
{
    float temp[3];
    CrossProduct(this->front,this->up, temp);
    NormalizeVector(temp, 3, temp);
    ScalarMultiplication(speed * 0.8, temp, 3, temp);
    Add(this->currentPosition, temp, 3, this->currentPosition);
}

/**
 * @brief Updates the position of the camera object.
 * 
 */
void Camera::UpdatePosition(KeyboardState *state) {
    float cameraSpeed = 0.2;

    if (state->up)
    {
        this->MoveUp(cameraSpeed);
    }

    if (state->down)
    {
        this->MoveDown(cameraSpeed);
    }

    if (state->left)
    {
        this->MoveLeft(cameraSpeed);
    }

    if (state->right)
    {
        this->MoveRight(cameraSpeed);
    }
}

/**
 * @brief Calculates the view matrix of the camera.
 * 
 * @param position The current position of the camera.
 * @param target The target to which the camera is pointing.
 * @param upVector The up vector of the camera.
 * @param result The resulting view matrix.
 */
void Camera::LookAt(float* position, float* target, float* upVector, float* result)
{
    float forward[3];
    Substract(position, target, 3, forward);
    NormalizeVector(forward, 3, forward);

    float left[3];
    CrossProduct(upVector, forward, left);
    NormalizeVector(left, 3, left);

    float up[3];
    CrossProduct(forward, left, up);

    // float matrix[16] =
    // {
    //     left[0], left[1], left[2], 0.0,
    //     up[0], up[1], up[2], 0.0,
    //     forward[0], forward[1], forward[2], 0.0,
    //     0.0, 0.0, 0.0, 1.0
    // };

    // float t[16];
    // SetTranslation(-position[0], -position[1], -position[2], t);
    // MultiplyMatrix(matrix, t, matrix);

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
