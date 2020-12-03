#include "camera.hpp"

/**
 * @brief Construct a new Camera object.
 * 
 */
Camera::Camera() :
    currentPosition{ 0.0, 0.0, -10.0 },
    direction{ 0.0, 0.0, 0.0 },
    up{ 0.0, 1.0, 0.0 },
    xAngle(-90.0f),
    yAngle(0.0f),
    fieldOfView(45.0)
{
}

/**
 * @brief Updates the field of view (also known als zoom) of the camera.
 * 
 * @param state The current state of the scroll wheel.
 */
void Camera::UpdateZoom(ScrollWheelState* state)
{
    this->fieldOfView = state->zoom;
    float aspect = winWidth / winHeight;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    SetPerspectiveMatrix(this->fieldOfView, aspect, nearPlane, farPlane, this->projectionMatrix);
}

/**
 * @brief Updates the view of the camera.
 * 
 * @remarks Based on the article at https://learnopengl.com/Getting-started/Camera
 */
void Camera::UpdateView()
{
    // Fixing the target to the point of origin.
    float target[3] = {
        0,
        0,
        0
    };
    
    Substract(this->currentPosition, target, 3, this->direction);
    NormalizeVector(this->direction, 3, this->direction);

    this->LookAt(this->direction);
}

/**
 * @brief Moves the camera up.
 * 
 * @param speed The speed factor that describes how fast the camera should move.
 */
void Camera::MoveUp(float speed)
{
    float temp[3];
    ScalarMultiplication(speed, this->direction, 3, temp);
    Add(this->currentPosition, temp, 3, this->currentPosition);
}

/**
 * @brief Moves the camera down.
 * 
 * @param speed The speed factor that describes how fast the camera should move.
 */
void Camera::MoveDown(float speed)
{
    float temp[3];
    ScalarMultiplication(speed, this->direction, 3, temp);
    Substract(this->currentPosition, temp, 3, this->currentPosition);
} 

/**
 * @brief Moves the camera left.
 * 
 * @param speed The speed factor that describes how fast the camera should move.
 */
void Camera::MoveLeft(float speed)
{
    float temp[3];
    CrossProduct(this->direction, this->up, temp);
    NormalizeVector(temp, 3, temp);
    ScalarMultiplication(speed, temp, 3, temp);
    Substract(this->currentPosition, temp, 3, this->currentPosition);
}

/**
 * @brief Moves the camera right.
 * 
 * @param speed The speed factor that describes how fast the camera should move.
 */
void Camera::MoveRight(float speed)
{
    float temp[3];
    CrossProduct(this->direction, this->up, temp);
    NormalizeVector(temp, 3, temp);
    ScalarMultiplication(speed, temp, 3, temp);
    Add(this->currentPosition, temp, 3, this->currentPosition);
}

/**
 * @brief Updates the position of the camera object.
 * 
 */
void Camera::UpdatePosition(KeyboardState *keyboardState, MouseState *mouseState) {
    float cameraSpeed = 0.2;

    if (keyboardState->currentLimb == 0)
    {
        if (keyboardState->up)
        {
            this->MoveUp(cameraSpeed);
        }

        if (keyboardState->down)
        {
            this->MoveDown(cameraSpeed);
        }

        if (keyboardState->left)
        {
            this->MoveLeft(cameraSpeed);
        }

        if (keyboardState->right)
        {
            this->MoveRight(cameraSpeed);
        }
    }

    this->xAngle = mouseState->xAngle;
    this->yAngle = mouseState->yAngle;

    this->UpdateView();
}

/**
 * @brief Calculates the view matrix of the camera.
 * 
 * @param position The current position of the camera.
 * @param target The target to which the camera is pointing.
 * @param upVector The up vector of the camera.
 * @param result The resulting view matrix.
 * @remarks Based on the article at https://www.geertarien.com/blog/2017/07/30/breakdown-of-the-lookAt-function-in-OpenGL/
 */
void Camera::LookAt(float* target)
{
    float zAxis[3];
    Substract(target, this->currentPosition, 3, zAxis);
    NormalizeVector(zAxis, 3, zAxis);

    float xAxis[3];
    CrossProduct(zAxis, this->up, xAxis);
    NormalizeVector(xAxis, 3, xAxis);

    float yAxis[3];
    CrossProduct(xAxis, zAxis, yAxis);
    Negate(zAxis, 3, zAxis);

    float matrix[16] =
    {
        xAxis[0], xAxis[1], xAxis[2], -DotProduct(xAxis, this->currentPosition, 3),
        yAxis[0], yAxis[1], yAxis[2], -DotProduct(yAxis, this->currentPosition, 3),
        zAxis[0], zAxis[1], zAxis[2], -DotProduct(zAxis, this->currentPosition, 3),
        0, 0, 0, 1
    };

    memcpy(this->viewMatrix, matrix, 16 * sizeof(float));
}
