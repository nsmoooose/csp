/*
		This class has only recieved cursorial testing.
		The functions seem to work though.  The Inverse
		functions were taken from Graphics Gems 4.
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "OpenGLMath.h"

void OpenGLMath::MatrixIdentity( _Matrix *matrix )
{
	matrix->_fz = 1.0f;
	matrix->_rx = matrix->_uy = matrix->_44 = 1.0f;
	matrix->_ux = matrix->_fx = matrix->_14 = matrix->_fy = matrix->_24 = matrix->_34 = 0.0f;
	matrix->_ry = matrix->_rz = matrix->_uz = matrix->_px = matrix->_py = matrix->_pz = 0.0f;
}

void OpenGLMath::MatrixTranslation( _Matrix *matrix, float x, float y, float z )
{
	matrix->_14 = x;
	matrix->_24 = y;
	matrix->_34 = z;
}

void OpenGLMath::MatrixRotationX( _Matrix *matrix, float x )
{
	matrix->_rx = matrix->_44 = 1.0f;
	matrix->_ux = matrix->_fx = matrix->_14 = matrix->_24 = matrix->_34 = 0.0f;
	matrix->_ry = matrix->_rz = matrix->_px = matrix->_py = matrix->_pz = 0.0f;
	matrix->_uy = (float)cos( x );
	matrix->_fy = (float)-sin( x );
	matrix->_uz = (float)sin( x );
	matrix->_fz = (float)cos( x );
}

void OpenGLMath::MatrixRotationY( _Matrix *matrix, float y )
{
	matrix->_uy = matrix->_44 = 1.0f;
	matrix->_ux = matrix->_14 = matrix->_ry = matrix->_fy = matrix->_24 = 0.0f;
	matrix->_uz = matrix->_34 = matrix->_px = matrix->_py = matrix->_pz = 0.0f;
	matrix->_rx = matrix->_fz = (float)cos( y );
	matrix->_fx = (float)sin( y );
	matrix->_rz = (float)-sin( y );
}

void OpenGLMath::MatrixRotationZ( _Matrix *matrix, float z )
{
	matrix->_fx = matrix->_14 = matrix->_fy = matrix->_24 = matrix->_34 = 0.0f;
	matrix->_rz = matrix->_uz = matrix->_px = matrix->_py = matrix->_pz = 0.0f;
	matrix->_fz = matrix->_44 = 1.0f;
	matrix->_rx = matrix->_uy = (float)cos( z );
	matrix->_ry = (float)sin( z );
	matrix->_ux = (float)-sin( z );
}

void OpenGLMath::MatrixPerspectiveFOV( _Matrix *matrix, short FOV, short width, short height, float nearz, float farz )
{
	matrix->_rx = (float)atan( FOV/2 ) / (width/height);
	matrix->_uy = (float)atan( FOV/2 );
	matrix->_fz = (nearz + farz) / (nearz - farz);
	matrix->_34 = (2.0f*nearz*farz) / (nearz - farz);
	matrix->_pz = -1.0f;
	matrix->_ux = matrix->_fx = matrix->_14 = matrix->_fy = matrix->_24 = matrix->_44 = 0.0f;
	matrix->_ry = matrix->_rz = matrix->_uz = matrix->_px = matrix->_py = 0.0f;
}

void OpenGLMath::MatrixMultiply( _Matrix *matrix, _Matrix *matrixsource )
{
	float *m, *ms;
	m = (float *)matrix;
	ms = (float *)matrixsource;
	*(m +  0) = *(m +  0) * *(ms +  0) + *(m +  4) * *(ms +  1) + *(m +  8) * *(ms +  2) + *(m + 12) * *(ms +  3);
	*(m +  1) = *(m +  1) * *(ms +  0) + *(m +  5) * *(ms +  1) + *(m +  9) * *(ms +  2) + *(m + 13) * *(ms +  3);
	*(m +  2) = *(m +  2) * *(ms +  0) + *(m +  6) * *(ms +  1) + *(m + 10) * *(ms +  2) + *(m + 14) * *(ms +  3);
	*(m +  3) = *(m +  3) * *(ms +  0) + *(m +  7) * *(ms +  1) + *(m + 11) * *(ms +  2) + *(m + 15) * *(ms +  3);
	*(m +  4) = *(m +  0) * *(ms +  4) + *(m +  4) * *(ms +  5) + *(m +  8) * *(ms +  6) + *(m + 12) * *(ms +  7);
	*(m +  5) = *(m +  1) * *(ms +  4) + *(m +  5) * *(ms +  5) + *(m +  9) * *(ms +  6) + *(m + 13) * *(ms +  7);
	*(m +  6) = *(m +  2) * *(ms +  4) + *(m +  6) * *(ms +  5) + *(m + 10) * *(ms +  6) + *(m + 14) * *(ms +  7);
	*(m +  7) = *(m +  3) * *(ms +  4) + *(m +  7) * *(ms +  5) + *(m + 11) * *(ms +  6) + *(m + 15) * *(ms +  7);
	*(m +  8) = *(m +  0) * *(ms +  8) + *(m +  4) * *(ms +  9) + *(m +  8) * *(ms + 10) + *(m + 12) * *(ms + 11);
	*(m +  9) = *(m +  1) * *(ms +  8) + *(m +  5) * *(ms +  9) + *(m +  9) * *(ms + 10) + *(m + 13) * *(ms + 11);
	*(m + 10) = *(m +  2) * *(ms +  8) + *(m +  6) * *(ms +  9) + *(m + 10) * *(ms + 10) + *(m + 14) * *(ms + 11);
	*(m + 11) = *(m +  3) * *(ms +  8) + *(m +  7) * *(ms +  9) + *(m + 11) * *(ms + 10) + *(m + 15) * *(ms + 11);
	*(m + 12) = *(m +  0) * *(ms + 12) + *(m +  4) * *(ms + 13) + *(m +  8) * *(ms + 14) + *(m + 12) * *(ms + 15);
	*(m + 13) = *(m +  1) * *(ms + 12) + *(m +  5) * *(ms + 13) + *(m +  9) * *(ms + 14) + *(m + 13) * *(ms + 15);
	*(m + 14) = *(m +  2) * *(ms + 12) + *(m +  6) * *(ms + 13) + *(m + 10) * *(ms + 14) + *(m + 14) * *(ms + 15);
	*(m + 15) = *(m +  3) * *(ms + 12) + *(m +  7) * *(ms + 13) + *(m + 11) * *(ms + 14) + *(m + 15) * *(ms + 15);
}

void OpenGLMath::MatrixMultiply2( _Matrix *matrix, _Matrix *matrixsource, _Matrix *matrixsource2 )
{
	float *m, *m1, *m2;
	m = (float *)matrix;
	m1 = (float *)matrixsource;
	m2 = (float *)matrixsource2;
	*(m +  0) = *(m1 +  0) * *(m2 +  0) + *(m1 +  4) * *(m2 +  1) + *(m1 +  8) * *(m2 +  2) + *(m1 + 12) * *(m2 +  3);
	*(m +  1) = *(m1 +  1) * *(m2 +  0) + *(m1 +  5) * *(m2 +  1) + *(m1 +  9) * *(m2 +  2) + *(m1 + 13) * *(m2 +  3);
	*(m +  2) = *(m1 +  2) * *(m2 +  0) + *(m1 +  6) * *(m2 +  1) + *(m1 + 10) * *(m2 +  2) + *(m1 + 14) * *(m2 +  3);
	*(m +  3) = *(m1 +  3) * *(m2 +  0) + *(m1 +  7) * *(m2 +  1) + *(m1 + 11) * *(m2 +  2) + *(m1 + 15) * *(m2 +  3);
	*(m +  4) = *(m1 +  0) * *(m2 +  4) + *(m1 +  4) * *(m2 +  5) + *(m1 +  8) * *(m2 +  6) + *(m1 + 12) * *(m2 +  7);
	*(m +  5) = *(m1 +  1) * *(m2 +  4) + *(m1 +  5) * *(m2 +  5) + *(m1 +  9) * *(m2 +  6) + *(m1 + 13) * *(m2 +  7);
	*(m +  6) = *(m1 +  2) * *(m2 +  4) + *(m1 +  6) * *(m2 +  5) + *(m1 + 10) * *(m2 +  6) + *(m1 + 14) * *(m2 +  7);
	*(m +  7) = *(m1 +  3) * *(m2 +  4) + *(m1 +  7) * *(m2 +  5) + *(m1 + 11) * *(m2 +  6) + *(m1 + 15) * *(m2 +  7);
	*(m +  8) = *(m1 +  0) * *(m2 +  8) + *(m1 +  4) * *(m2 +  9) + *(m1 +  8) * *(m2 + 10) + *(m1 + 12) * *(m2 + 11);
	*(m +  9) = *(m1 +  1) * *(m2 +  8) + *(m1 +  5) * *(m2 +  9) + *(m1 +  9) * *(m2 + 10) + *(m1 + 13) * *(m2 + 11);
	*(m + 10) = *(m1 +  2) * *(m2 +  8) + *(m1 +  6) * *(m2 +  9) + *(m1 + 10) * *(m2 + 10) + *(m1 + 14) * *(m2 + 11);
	*(m + 11) = *(m1 +  3) * *(m2 +  8) + *(m1 +  7) * *(m2 +  9) + *(m1 + 11) * *(m2 + 10) + *(m1 + 15) * *(m2 + 11);
	*(m + 12) = *(m1 +  0) * *(m2 + 12) + *(m1 +  4) * *(m2 + 13) + *(m1 +  8) * *(m2 + 14) + *(m1 + 12) * *(m2 + 15);
	*(m + 13) = *(m1 +  1) * *(m2 + 12) + *(m1 +  5) * *(m2 + 13) + *(m1 +  9) * *(m2 + 14) + *(m1 + 13) * *(m2 + 15);
	*(m + 14) = *(m1 +  2) * *(m2 + 12) + *(m1 +  6) * *(m2 + 13) + *(m1 + 10) * *(m2 + 14) + *(m1 + 14) * *(m2 + 15);
	*(m + 15) = *(m1 +  3) * *(m2 + 12) + *(m1 +  7) * *(m2 + 13) + *(m1 + 11) * *(m2 + 14) + *(m1 + 15) * *(m2 + 15);
}
void OpenGLMath::MatrixInverse( _Matrix *matrix )
{
	float swap[4];
	float *m1, *m2;
	_Matrix tmp;
	MatrixIdentity( &tmp );
	m1 = (float *)matrix;
	m2 = (float *)&tmp;
	int i, j, i1;

	for( j = 0; j < 4; j++ )
	{
		i1 = j;
		for( i = j + 1; i < 4; i++ )
		{
			if( fabs( *(m1+(i*4)+j) ) > fabs( *(m1+(i1*4)+j) ) )
			{
				i1 = i;
			}
		}
		swap[0] = *(m1+(i1*4)+0);
		swap[1] = *(m1+(i1*4)+1);
		swap[2] = *(m1+(i1*4)+2);
		swap[3] = *(m1+(i1*4)+3);
		*(m1+(i1*4)+0) = *(m1+(j*4)+0);
		*(m1+(i1*4)+1) = *(m1+(j*4)+1);
		*(m1+(i1*4)+2) = *(m1+(j*4)+2);
		*(m1+(i1*4)+3) = *(m1+(j*4)+3);
		*(m1+(j*4)+0) = swap[0];
		*(m1+(j*4)+1) = swap[1];
		*(m1+(j*4)+2) = swap[2];
		*(m1+(j*4)+3) = swap[3];
		swap[0] = *(m2+(i1*4)+0);
		swap[1] = *(m2+(i1*4)+1);
		swap[2] = *(m2+(i1*4)+2);
		swap[3] = *(m2+(i1*4)+3);
		*(m2+(i1*4)+0) = *(m2+(j*4)+0);
		*(m2+(i1*4)+1) = *(m2+(j*4)+1);
		*(m2+(i1*4)+2) = *(m2+(j*4)+2);
		*(m2+(i1*4)+3) = *(m2+(j*4)+3);
		*(m2+(j*4)+0) = swap[0];
		*(m2+(j*4)+1) = swap[1];
		*(m2+(j*4)+2) = swap[2];
		*(m2+(j*4)+3) = swap[3];

		*(m2+(j*4)+0) /= *(m1+(j*4)+j);
		*(m2+(j*4)+1) /= *(m1+(j*4)+j);
		*(m2+(j*4)+2) /= *(m1+(j*4)+j);
		*(m2+(j*4)+3) /= *(m1+(j*4)+j);
		*(m1+(j*4)+0) /= *(m1+(j*4)+j);
		*(m1+(j*4)+1) /= *(m1+(j*4)+j);
		*(m1+(j*4)+2) /= *(m1+(j*4)+j);
		*(m1+(j*4)+3) /= *(m1+(j*4)+j);

		for( i = 0; i < 4; i++ )
		{
			if( i != j )
			{
				*(m2+(i*4)+0) -= *(m1+(i*4)+j) * *(m2+(j*4)+0);
				*(m2+(i*4)+1) -= *(m1+(i*4)+j) * *(m2+(j*4)+1);
				*(m2+(i*4)+2) -= *(m1+(i*4)+j) * *(m2+(j*4)+2);
				*(m2+(i*4)+3) -= *(m1+(i*4)+j) * *(m2+(j*4)+3);
				*(m1+(i*4)+0) -= *(m1+(i*4)+j) * *(m1+(j*4)+0);
				*(m1+(i*4)+1) -= *(m1+(i*4)+j) * *(m1+(j*4)+1);
				*(m1+(i*4)+2) -= *(m1+(i*4)+j) * *(m1+(j*4)+2);
				*(m1+(i*4)+3) -= *(m1+(i*4)+j) * *(m1+(j*4)+3);
			}
		}
	}
	matrix = &tmp;
}

void OpenGLMath::MatrixInverse2( _Matrix *matrix, _Matrix *source )
{
	float swap[4];
	float *m1, *m2;
	_Matrix tmp;
	memcpy( &tmp, source, sizeof( _Matrix ) );
	m1 = (float *)&tmp;
	m2 = (float *)matrix;
	int i, j, i1;

	for( j = 0; j < 4; j++ )
	{
		i1 = j;
		for( i = j + 1; i < 4; i++ )
		{
			if( fabs( *(m1+(i*4)+j) ) > fabs( *(m1+(i1*4)+j) ) )
			{
				i1 = i;
			}
		}
		swap[0] = *(m1+(i1*4)+0);
		swap[1] = *(m1+(i1*4)+1);
		swap[2] = *(m1+(i1*4)+2);
		swap[3] = *(m1+(i1*4)+3);
		*(m1+(i1*4)+0) = *(m1+(j*4)+0);
		*(m1+(i1*4)+1) = *(m1+(j*4)+1);
		*(m1+(i1*4)+2) = *(m1+(j*4)+2);
		*(m1+(i1*4)+3) = *(m1+(j*4)+3);
		*(m1+(j*4)+0) = swap[0];
		*(m1+(j*4)+1) = swap[1];
		*(m1+(j*4)+2) = swap[2];
		*(m1+(j*4)+3) = swap[3];
		swap[0] = *(m2+(i1*4)+0);
		swap[1] = *(m2+(i1*4)+1);
		swap[2] = *(m2+(i1*4)+2);
		swap[3] = *(m2+(i1*4)+3);
		*(m2+(i1*4)+0) = *(m2+(j*4)+0);
		*(m2+(i1*4)+1) = *(m2+(j*4)+1);
		*(m2+(i1*4)+2) = *(m2+(j*4)+2);
		*(m2+(i1*4)+3) = *(m2+(j*4)+3);
		*(m2+(j*4)+0) = swap[0];
		*(m2+(j*4)+1) = swap[1];
		*(m2+(j*4)+2) = swap[2];
		*(m2+(j*4)+3) = swap[3];

		*(m2+(j*4)+0) /= *(m1+(j*4)+j);
		*(m2+(j*4)+1) /= *(m1+(j*4)+j);
		*(m2+(j*4)+2) /= *(m1+(j*4)+j);
		*(m2+(j*4)+3) /= *(m1+(j*4)+j);
		*(m1+(j*4)+0) /= *(m1+(j*4)+j);
		*(m1+(j*4)+1) /= *(m1+(j*4)+j);
		*(m1+(j*4)+2) /= *(m1+(j*4)+j);
		*(m1+(j*4)+3) /= *(m1+(j*4)+j);

		for( i = 0; i < 4; i++ )
		{
			if( i != j )
			{
				*(m2+(i*4)+0) -= *(m1+(i*4)+j) * *(m2+(j*4)+0);
				*(m2+(i*4)+1) -= *(m1+(i*4)+j) * *(m2+(j*4)+1);
				*(m2+(i*4)+2) -= *(m1+(i*4)+j) * *(m2+(j*4)+2);
				*(m2+(i*4)+3) -= *(m1+(i*4)+j) * *(m2+(j*4)+3);
				*(m1+(i*4)+0) -= *(m1+(i*4)+j) * *(m1+(j*4)+0);
				*(m1+(i*4)+1) -= *(m1+(i*4)+j) * *(m1+(j*4)+1);
				*(m1+(i*4)+2) -= *(m1+(i*4)+j) * *(m1+(j*4)+2);
				*(m1+(i*4)+3) -= *(m1+(i*4)+j) * *(m1+(j*4)+3);
			}
		}
	}
}

void OpenGLMath::MatrixScale( _Matrix *matrix, float x, float y, float z )
{
	matrix->_rx = x;
	matrix->_uy = y;
	matrix->_fz = z;
	matrix->_ux = matrix->_fx = matrix->_14 = matrix->_fy = matrix->_24 = matrix->_34 = matrix->_44 = 0.0f;
	matrix->_ry = matrix->_rz = matrix->_uz = matrix->_px = matrix->_py = matrix->_pz = 0.0f;
}

void OpenGLMath::SetVector( _Vector *vector, float x, float y, float z )
{
	vector->x = x;
	vector->y = y;
	vector->z = z;
}

_Vector OpenGLMath::CalculateNormal( _Vector *Base, _Vector *Left, _Vector *Right )
{
	_Vector vec1, vec2, vec3;
	vec1.x = Left->x - Base->x;
	vec1.y = Left->y - Base->y;
	vec1.z = Left->z - Base->z;
	vec2.x = Right->x - Base->x;
	vec2.y = Right->y - Base->y;
	vec2.z = Right->z - Base->z;
	CrossProduct( &vec3, &vec1, &vec2 );
	Normalize( &vec3 );
	return(vec3);
}

_Vector OpenGLMath::CrossProduct( _Vector *forward, _Vector *up )
{
	_Vector vec1;
	vec1.x = forward->y * up->z - forward->z * up->y;
	vec1.y = forward->z * up->x - forward->x * up->z;
	vec1.z = forward->x * up->y - forward->y * up->x;
	Normalize( &vec1 );
	return(vec1);
}

void OpenGLMath::CrossProduct( _Vector *vec, _Vector *forward, _Vector *up )
{
	*vec = CrossProduct( forward, up );
}

float OpenGLMath::ToDegree( float radian )
{
	return( radian * 180 / (float)PI );
}

float OpenGLMath::ToRadian( float degree )
{
	return( degree * (float)PI / 180 );
}

void OpenGLMath::Normalize( _Vector *vec )
{
	float len = Length( vec );
	if( len == 0 )
	{
		len = 1.0f;
	}
	vec->x /= len;
	vec->y /= len;
	vec->z /= len;
}

float OpenGLMath::Length( _Vector *vec )
{
	return( (float)(sqrt((vec->x * vec->x) + (vec->y * vec->y) + (vec->z * vec->z) )) );
}