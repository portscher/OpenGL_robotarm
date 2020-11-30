/******************************************************************
*
* Matrix.h
*
* Description: Helper routine for matrix computations.
*
* Interactive Graphics and Simulation Group
* Institute of Computer Science
* University of Innsbruck
*
*******************************************************************/


#ifndef __MATRIX_H__
#define __MATRIX_H__

void SetIdentityMatrix(float* result);
void SetScaleMatrix(float scalex, float scaley,float scalez,float* result);  
void SetRotationX(float anglex, float* result);
void SetRotationY(float angley, float* result);
void SetRotationZ(float anglez, float* result);
void SetTranslation(float x, float y, float z, float* result);
void MultiplyMatrix(float* m1, float* m2, float* result);
void SetPerspectiveMatrix(float fov, float aspect, float nearPlane, float farPlane, float* result);

#endif
