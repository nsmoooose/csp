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


#if !defined(VECTOR_H)
#define VECTOR_H





class CVector
{

public:
	CVector ();
	CVector (float inX, float inY, float inZ);
	CVector(const CVector &v);
	virtual ~CVector();

   ///////////////////////
   // additional operators
   ///////////////////////
	CVector       &operator = (const CVector *a);
   
	friend CVector operator + (const CVector &a, const CVector &b);  // add a and b (a + b)
	friend CVector operator - (const CVector &a, const CVector &b);  // subtract b from a ( a - b)
	friend CVector operator % (const CVector &a, const CVector &b);  // cross product of a and b (a % b)
	friend float   operator * (const CVector &a, const CVector &b);  // dot product of a and b (a * b)
	friend bool    operator == (const CVector &a, const CVector &b);
	friend bool    operator != (const CVector &a, const CVector &b);
	friend CVector operator - (const CVector &a);
	friend CVector operator + (const CVector &a, const CVector &b);
	friend CVector operator * (const CVector &v, float f);
	friend CVector operator * (float f, const CVector &v);
	friend CVector operator / (const CVector &v, float f);

	CVector &operator += (const CVector &v);
	CVector &operator -= (const CVector &v);
	CVector &operator *= (float f);
	CVector &operator /= (float f);
  
	/////////////////////
	// additional methods
	/////////////////////

	// GetDirection.
	// get direction of vector
	void GetDirection ();

	// SetMagnitudeOfVector.
	// set the magnitude of the vector (without modifying
	// direction, apportioned across the existing vector's magnitudes)
	void SetMagnitudeOfVector (float velocity);

	// GetDist.
	// get distance between two points.
	float GetDist (CVector pos1, CVector pos2);

	// GetAngleBetween.
	// get angle between two vectors.
	float GetAngleBetween (CVector vec1, CVector vec2);

	void set(float xIn, float yIn, float zIn);
	float length() const;
	float lengthSqr() const;
	bool isZero() const;
	CVector &normalize();
	float DotProduct(const CVector &a, const CVector &b);

	// Debug
	void                  fprint(char* str) const;

	float x, y, z;
};

#endif
