///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   class represents a three-dimensional vector
//   original coding by Steve Rabin (Game Programming Gems)
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	6/14/2001	adjusted original version to fit into the terrain engine - zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "Vector.h"




CVector::CVector ()
{
	x = y = z = 0.0f;
}

CVector::CVector (float inX, float inY, float inZ)
{
	x = inX;
	y = inY;
	z = inZ;
}

CVector::CVector(const CVector &v) 
{
	x = v.x;
	y = v.y;
	z = v.z;
}

CVector::~CVector ()
{
}

///////////////////////
// additional operators
///////////////////////

// Assign.
// Put contents of v into vector.
CVector &CVector::operator = (const CVector *v) 
{
	x = v->x;
	y = v->y;
	z = v->z;

	return *this;
}

// Add.
// Add two vectors
CVector operator + (const CVector &a, const CVector &b) 
{
	CVector ret(a);
	ret += b;

	return ret;
}

// Subtract.
// Subtract one vector from another
CVector operator - (const CVector &a, const CVector &b) 
{
	CVector ret(a);
	ret -= b;

	return ret;
}

// Cross Product.
// Compute cross product of the two vectors.
CVector operator % (const CVector &a, const CVector &b)
{
	CVector c(a.y*b.z - a.z*b.y,   // x component
              a.z*b.x - a.x*b.z,   // y component
              a.x*b.y - a.y*b.x);  // z component
  
	return c;
}

// Dot Product.
// Compute dot product of the two vectors.  (Yes, this duplicates
// something already in mtxlib.h, but it's more compact syntatically.)
float operator * (const CVector &a, const CVector &b) 
{
	return (a.x*b.x + a.y*b.y + a.z*b.z);
}

// Are these two CVector's equal?
bool operator == (const CVector &a, const CVector &b) 
{
  return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z));
}

// Are these two CVector's not equal?
bool operator != (const CVector &a, const CVector &b) 
{
  return ((a.x != b.x) || (a.y != b.y) || (a.z != b.z));
}

// Negate a CVector
CVector operator - (const CVector &a) 
{
  return CVector(-a.x, -a.y, -a.z);
}

// Multiply CVector by a float
CVector operator * (const CVector &v, float f) 
{
  return CVector(f * v.x, f * v.y, f * v.z);
}

// Multiply CVector by a float
CVector operator * (float f, const CVector &v) 
{
  return CVector(f * v.x, f * v.y, f * v.z);
}

// Divide CVector by a float
CVector operator / (const CVector &v, float f) 
{
  return CVector(v.x / f, v.y / f, v.z / f);
}

// Add a CVector to this one
CVector &CVector::operator += (const CVector &v) 
{
  x += v.x;
  y += v.y;
  z += v.z;

  return *this;
}

// Subtract a CVector from this one
CVector &CVector::operator -= (const CVector &v) 
{
  x -= v.x;
  y -= v.y;
  z -= v.z;

  return *this;
}

// Multiply the CVector by a float
CVector &CVector::operator *= (float f) 
{
  x *= f;
  y *= f;
  z *= f;

  return *this;
}

// Divide the CVector by a float
CVector &CVector::operator /= (float f) 
{
  x /= f;
  y /= f;
  z /= f;

  return *this;
}

/////////////////////
// additional methods
/////////////////////

// get direction of vector
void CVector::GetDirection ()
{
	this->normalize();
}

// ste the magnitude of the vector (without modifying
// direction, apportioned across the existing vector's magnitudes)
void CVector::SetMagnitudeOfVector (float velocity)
{
	this->normalize();

	x *= velocity;
	y *= velocity;
	z *= velocity;
}

// get distance between two points
float CVector::GetDist (CVector pos1, CVector pos2)
{
   CVector  temp = pos1 - pos2;

   return temp.length();

}

// get angle between two vectors
float CVector::GetAngleBetween (CVector vec1, CVector vec2) 
{

   float mag1 = vec1.length();
   float mag2 = vec2.length();
   float dotproduct = DotProduct(vec1, vec2);

   return (float) (acos (dotproduct/(mag1 * mag2)));
}

// Dot product of two CVector's
float CVector::DotProduct(const CVector &a, const CVector &b) 
{
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

// Set Values
void CVector::set(float xIn, float yIn, float zIn)
{
  x = xIn;
  y = yIn;
  z = zIn;
}

// Get length of a CVector
float CVector::length() const 
{
  return (float) sqrt(x*x + y*y + z*z);
}

// Get squared length of a CVector
float CVector::lengthSqr() const 
{
  return (x*x + y*y + z*z);
}

// Does CVector equal (0, 0, 0)?
bool CVector::isZero() const 
{
  return ((x == 0.0F) && (y == 0.0F) && (z == 0.0F));
}

// Normalize a CVector
CVector &CVector::normalize() 
{
  float m = length();

  if (m > 0.0F) 
    m = 1.0F / m;
  else 
    m = 0.0F;
  x *= m;
  y *= m;
  z *= m;

  return *this;
}