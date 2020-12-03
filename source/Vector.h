/******************************************************************
*
* Vector.h
*
* Description: Helper routine for vector computations.
*
* Interactive Graphics and Simulation Group
* Institute of Computer Science
* University of Innsbruck
*
*******************************************************************/


#ifndef __VECTOR_H__
#define __VECTOR_H__

typedef struct
{
        float x;
        float y;
        float z;
} Vector;

float Norm(Vector v);
Vector Normalize(Vector v);
float Dot(Vector v1, Vector v2);
Vector Cross(Vector v1, Vector v2);

#endif // __VECTOR_H__
