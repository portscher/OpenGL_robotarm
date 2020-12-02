#include "camera.hpp"

Camera::Camera() :
    currentPosition{ 0.0, 5.0, -25.0 },
    front{ 0.0, 0.0, 1.0 },
    up{ 0.0, 0.0, 1.0 },
    xAngle(-15.0),
    yAngle(0.0),
    lookingAt(0.0),
    fieldOfView(45.0)
{
}

void Camera::UpdateView()
{
    float target[3] = {
        0,
        0,
        0
    };
    // this->front[0] = cosf(ToRadian(this->xAngle)) * sinf(ToRadian(this->yAngle));
    // this->front[1] = sinf(ToRadian(this->xAngle));
    // this->front[2] = cosf(ToRadian(this->xAngle)) * sinf(ToRadian(this->yAngle));
    // NormalizeVector(this->front, 3, this->front);
    // Add(this->currentPosition, this->front, 3, target);

    printf("pos: [%g,%g, %g]\nfront: [%g, %g, %g]\nlookingat: [%g, %g, %g]\n",
    this->currentPosition[0], this->currentPosition[1], this->currentPosition[2],
    this->front[0], this->front[1], this->front[2],
    target[0], target[1], target[2]);

    this->LookAt(target);

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

    if (state->currentLimb == 0) {
        if (state->up && state->currentLimb == 0)
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
}

/**
 * @brief Calculates the view matrix of the camera.
 * 
 * @param position The current position of the camera.
 * @param target The target to which the camera is pointing.
 * @param upVector The up vector of the camera.
 * @param result The resulting view matrix.
 */
void Camera::LookAt(float* target)
{
    float forward[3];
    Substract(this->currentPosition, target, 3, forward);
    NormalizeVector(forward, 3, forward);

    float left[3];
    CrossProduct(this->up, forward, left);
    NormalizeVector(left, 3, left);

    float up[3];
    CrossProduct(forward, left, up);

    float matrix[16] =
    {
        left[0], left[1], left[2], 0.0,
        up[0], up[1], up[2], 0.0,
        forward[0], forward[1], forward[2], 0.0,
        0.0, 0.0, 0.0, 1.0
    };

    float t[16];
    SetTranslation(-this->currentPosition[0], -this->currentPosition[1], -this->currentPosition[2], t);
    MultiplyMatrix(matrix, t, matrix);

    memcpy(this->viewMatrix, matrix, 16 * sizeof(float));
}
