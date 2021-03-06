/******************************************************************
*
* Matrix.c
*
* Description: Helper routine for matrix computations.
*
* Interactive Graphics and Simulation Group
* Institute of Computer Science
* University of Innsbruck
*
*******************************************************************/

/* Standard includes */
#include <cstring>
#include <cmath>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


/******************************************************************
*
* @brief Initializes an identity matrix and saves it in the given matrix
*
* @param result
*
*******************************************************************/

void SetIdentityMatrix(float *result)
{
    float identity[16] = {
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0,
    };

    memcpy(result, identity, 16 * sizeof(float));
}

/******************************************************************
*
* @brief Sets a ScaleMatrix
 *
 * @param scalex = Scaling value for the x axis
 * @param scaley = Scaling value for the y axis
 * @param scalez = Scaling value for the z axis
 * @param result = the matrix where the result is saved
*
*******************************************************************/

void SetScaleMatrix(float scalex, float scaley, float scalez, float *result)
{
    float scale[16] = {
            scalex, 0.0, 0.0, 0.0,
            0.0, scaley, 0.0, 0.0,
            0.0, 0.0, scalez, 0.0,
            0.0, 0.0, 0.0, 1.0,
    };

    memcpy(result, scale, 16 * sizeof(float));
}

/******************************************************************
*
* SetRotationX
*
*******************************************************************/

void SetRotationX(float anglex, float *result)
{
    anglex = M_PI / 180 * anglex;   /* Conversion angle from degree to radians */

    float temp[16] =
            {
                    1.0, 0.0, 0.0, 0.0,
                    0.0, cosf(anglex), -sinf(anglex), 0.0,
                    0.0, sinf(anglex), cosf(anglex), 0.0,
                    0.0, 0.0, 0.0, 1.0
            };

    memcpy(result, temp, 16 * sizeof(float));
}


/******************************************************************
*
* SetRotationY
*
*******************************************************************/

void SetRotationY(float angley, float *result)
{
    angley = M_PI / 180 * angley;   /* Conversion angle from degree to radians */

    float temp[16] =
            {
                    cosf(angley), 0.0, sinf(angley), 0.0,
                    0.0, 1.0, 0.0, 0.0,
                    -sinf(angley), 0.0, cosf(angley), 0.0,
                    0.0, 0.0, 0.0, 1.0
            };

    memcpy(result, temp, 16 * sizeof(float));
}


/******************************************************************
*
* SetRotationZ
*
*******************************************************************/

void SetRotationZ(float anglez, float *result)
{
    anglez = M_PI / 180 * anglez;   /* Conversion angle from degree to radian */

    float temp[16] =
            {
                    cosf(anglez), -sinf(anglez), 0.0, 0.0,
                    sinf(anglez), cosf(anglez), 0.0, 0.0,
                    0.0, 0.0, 1.0, 0.0,
                    0.0, 0.0, 0.0, 1.0
            };

    memcpy(result, temp, 16 * sizeof(float));
}


/******************************************************************
*
* SetTranslation
*
*******************************************************************/

void SetTranslation(float x, float y, float z, float *result)
{
    float temp[16] =
            {
                    1.0, 0.0, 0.0, x,
                    0.0, 1.0, 0.0, y,
                    0.0, 0.0, 1.0, z,
                    0.0, 0.0, 0.0, 1.0
            };

    memcpy(result, temp, 16 * sizeof(float));
}


/******************************************************************
*
* MultiplyMatrix
*
*******************************************************************/

void MultiplyMatrix(float *m1, float *m2, float *result)
{
    int i;
    float temp[16];

    for (i = 0; i < 16; i++)
        temp[i] = 0.0;

    temp[0] = m1[0] * m2[0] + m1[1] * m2[4] + m1[2] * m2[8] + m1[3] * m2[12];
    temp[1] = m1[0] * m2[1] + m1[1] * m2[5] + m1[2] * m2[9] + m1[3] * m2[13];
    temp[2] = m1[0] * m2[2] + m1[1] * m2[6] + m1[2] * m2[10] + m1[3] * m2[14];
    temp[3] = m1[0] * m2[3] + m1[1] * m2[7] + m1[2] * m2[11] + m1[3] * m2[15];

    temp[4] = m1[4] * m2[0] + m1[5] * m2[4] + m1[6] * m2[8] + m1[7] * m2[12];
    temp[5] = m1[4] * m2[1] + m1[5] * m2[5] + m1[6] * m2[9] + m1[7] * m2[13];
    temp[6] = m1[4] * m2[2] + m1[5] * m2[6] + m1[6] * m2[10] + m1[7] * m2[14];
    temp[7] = m1[4] * m2[3] + m1[5] * m2[7] + m1[6] * m2[11] + m1[7] * m2[15];

    temp[8] = m1[8] * m2[0] + m1[9] * m2[4] + m1[10] * m2[8] + m1[11] * m2[12];
    temp[9] = m1[8] * m2[1] + m1[9] * m2[5] + m1[10] * m2[9] + m1[11] * m2[13];
    temp[10] = m1[8] * m2[2] + m1[9] * m2[6] + m1[10] * m2[10] + m1[11] * m2[14];
    temp[11] = m1[8] * m2[3] + m1[9] * m2[7] + m1[10] * m2[11] + m1[11] * m2[15];

    temp[12] = m1[12] * m2[0] + m1[13] * m2[4] + m1[14] * m2[8] + m1[15] * m2[12];
    temp[13] = m1[12] * m2[1] + m1[13] * m2[5] + m1[14] * m2[9] + m1[15] * m2[13];
    temp[14] = m1[12] * m2[2] + m1[13] * m2[6] + m1[14] * m2[10] + m1[15] * m2[14];
    temp[15] = m1[12] * m2[3] + m1[13] * m2[7] + m1[14] * m2[11] + m1[15] * m2[15];

    memcpy(result, temp, 16 * sizeof(float));
}


/******************************************************************
*
* SetPerspectiveMatrix
*
*******************************************************************/

void SetPerspectiveMatrix(float fov, float aspect, float nearPlane,
                          float farPlane, float *result)
{
    float f = 1.0 / tan(fov * 3.1415926 / 360.0);
    float c1 = -(farPlane + nearPlane) / (farPlane - nearPlane);
    float c2 = -(2.0 * farPlane * nearPlane) / (farPlane - nearPlane);

    float temp[16] =
            {
                    f / aspect, 0.0, 0.0, 0.0,
                    0.0, f, 0.0, 0.0,
                    0.0, 0.0, c1, c2,
                    0.0, 0.0, -1.0, 0.0
            };

    memcpy(result, temp, 16 * sizeof(float));
}

/**
 * @brief Multiplies the given scalar with the given vector.
 * 
 * @param scalar The scalar value.
 * @param vector The vector that should be multiplied by the scalar.
 * @param vectorSize The size of the vector.
 * @param result The array where the result of the multiplication will be stored.
 */
void ScalarMultiplication(float scalar, float *vector, int vectorSize, float *result)
{
    float multiplication[vectorSize];
    for (int i = 0; i < vectorSize; i++)
    {
        multiplication[i] = vector[i] * scalar;
    }

    memcpy(result, multiplication, vectorSize * sizeof(float));
}

/**
 * @brief Adds two matrices.
 * 
 * @param a First matrix
 * @param b Another matrix
 * @param matrixSize The size of each matrix itself.
 * @param result The array where the result of the addition will be stored.
 */
void Add(float *a, float *b, int matrixSize, float *result)
{
    float addition[matrixSize];
    for (int i = 0; i < matrixSize; i++)
    {
        addition[i] = a[i] + b[i];
    }

    memcpy(result, addition, matrixSize * sizeof(float));
}

/**
 * @brief Computes the dot product of two matrices.
 * 
 * @param a First matrix.
 * @param b Another matrix
 * @param matrixSize The size of both matrices.
 * @return float The resulting dot product of both matrices.
 * @remarks Formular taken from https://en.wikipedia.org/wiki/Dot_product
 */
float DotProduct(float *a, float *b, int matrixSize)
{
    float product = 0;
    for (int i = 0; i < matrixSize; i++)
    {
        product += a[i] * b[i];
    }

    return product;
}

/**
 * @brief Negates the given vector.
 * 
 * @param vector The vector to negate.
 * @param vectorSize The size of the vector.
 * @param result The array where the negated vector will be stored.
 */
void Negate(float *vector, int vectorSize, float *result)
{
    float negate[vectorSize];
    for (int i = 0; i < vectorSize; i++)
    {
        negate[i] = -vector[i];
    }

    memcpy(result, negate, vectorSize * sizeof(float));
}

/**
 * @brief Substracts two matrices.
 * 
 * @param a First matrix.
 * @param b Another matrix.
 * @param matrixSize The size of each matrix itself.
 * @param result The array where the result of the substraction will be stored.
 */
void Substract(float *a, float *b, int matrixSize, float *result)
{
    float substraction[matrixSize];
    for (int i = 0; i < matrixSize; i++)
    {
        substraction[i] = a[i] - b[i];
    }

    memcpy(result, substraction, matrixSize * sizeof(float));
}

/**
 * @brief Calculates the cross product of two matrices.
 * 
 * @param a First matrix.
 * @param b Another matrix.
 * @param result The array where the result of the cross product will be stored.
 * @note The used formular for caluclating the cross product of two matrices 
 * can be found at https://de.wikipedia.org/wiki/Kreuzprodukt
 */
void CrossProduct(float *a, float *b, float *result)
{
    float crossProduct[3] =
            {
                    (a[1] * b[2]) - (a[2] * b[1]),
                    (a[2] * b[0]) - (a[0] * b[2]),
                    (a[0] * b[1]) - (a[1] * b[0]),
            };

    memcpy(result, crossProduct, 3 * sizeof(float));
}

/**
 * @brief Normalizes the given vector.
 * 
 * @param vector The vector that should be normalized.
 * @param vectorSize The size of the vector.
 * @param result The array that represents the normalized vector.
 */
void NormalizeVector(float *vector, int vectorSize, float *result)
{
    double sum = 0;
    for (int i = 0; i < vectorSize; i++)
    {
        sum += vector[i] * vector[i];
    }

    float length = sqrt(sum);
    float normalized[vectorSize];
    for (int i = 0; i < vectorSize; i++)
    {
        if (length == 0)
        {
            normalized[i] = 0;
            continue;
        }
        normalized[i] = vector[i] / length;
    }

    memcpy(result, normalized, vectorSize * sizeof(float));
}

/**
 * @brief Converts the given angle in degree to radian.
 * 
 * @param degree The angle in degree to convert.
 * @return float The angle in radian.
 */
float ToRadian(float degree)
{
    float radian = (M_PI / 180) * degree;
    return radian;
}
