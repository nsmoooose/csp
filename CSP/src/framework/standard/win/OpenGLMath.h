#ifndef	__OPENGLMATH_H__
#define	__OPENGLMATH_H__

#define	PI	3.141593

#include "standard/StandardMath.h"

class OpenGLMath : public StandardMath
{
public:
	void	MatrixIdentity( _Matrix * );
	void	MatrixTranslation( _Matrix *, float, float, float );
	void	MatrixRotationY( _Matrix *, float );
	void	MatrixRotationX( _Matrix *, float );
	void	MatrixRotationZ( _Matrix *, float );
	void	MatrixPerspectiveFOV( _Matrix *, short, short, short, float, float );
	void	MatrixMultiply( _Matrix *, _Matrix *);
	void	MatrixMultiply2( _Matrix *, _Matrix *, _Matrix * );
	void	MatrixInverse( _Matrix * );
	void	MatrixInverse2( _Matrix *, _Matrix * );
	void	MatrixScale( _Matrix *, float, float, float );
	void	SetVector( _Vector *, float, float, float );
	_Vector	CalculateNormal( _Vector *, _Vector *, _Vector * );
	_Vector	CrossProduct( _Vector *, _Vector * );
	void	CrossProduct( _Vector *, _Vector *, _Vector * );
	float	ToDegree( float );
	float	ToRadian( float );

private:
	float	Length( _Vector * );
	void	Normalize( _Vector * );
};

#endif