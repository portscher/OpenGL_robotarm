/******************************************************************
*
* Vector.c
*
* Description: Helper routine for vector computations.
*
* Interactive Graphics and Simulation Group
* Institute of Computer Science
* University of Innsbruck
*
*******************************************************************/

#include <cmath>

#include "Vector.hpp"

/******************************************************************
*
* Norm: compute norm (i.e. length) of vector v
*
*******************************************************************/

float Norm(Vector v)
{
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}


/******************************************************************
*
* Normalize: transform v into unit vector
*
*******************************************************************/

Vector Normalize(Vector v)
{
    Vector res;
    float norm = Norm(v);
    if(norm > 0.0f)
    {
        res.x = v.x / norm;
        res.y = v.y / norm;
        res.z = v.z / norm;
    }
    else
    {
        res.x = 0.0f;
        res.y = 0.0f;
        res.z = 0.0f;
    }
    return res;
}


/******************************************************************
*
* Dot: compute dot product between vectors v1 and v2
*
*******************************************************************/
float Dot(Vector v1, Vector v2)
{
    return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}


/******************************************************************
*
* Cross: compute cross product between vectors v1 and v2
*
*******************************************************************/
Vector Cross(Vector v1, Vector v2)
{
    Vector res;
    res.x = v1.y * v2.z - v1.z * v2.y;
    res.y = v1.z * v2.x - v1.x * v2.z;
    res.z = v1.x * v2.y - v1.y * v2.x;
    return res;
}


