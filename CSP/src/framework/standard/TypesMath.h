#ifndef __TYPESMATH_H__
#define __TYPESMATH_H__

// Available Preprocessors for Input
// DirectMath        - D3DX Math
// SimpleMath        - non-accerlated Math



// list of API's
const short _NOMATH       = -1;
const short _DIRECTMATH   = 0;
const short _SIMPLEMATH   = 1;
const short _OPENGLMATH   = 2;

// if we compiled with DirectMath make it default.
#ifdef DirectMath
  const short DEFAULT_MATH = _DIRECTMATH;

  #define MATH_HAS_A_DEFAULT
#endif


// if nothing else then default to nothing.
#ifndef MATH_HAS_A_DEFAULT
  const short DEFAULT_MATH = _SIMPLEMATH;
#endif

typedef struct _VectorStruct
{
    float x;
    float y;
    float z;
} _Vector;

typedef struct _MatrixStruct 
{
  //  struct 
  //  {
      float        _rx, _ux, _fx, _14;
      float        _ry, _uy, _fy, _24;
      float        _rz, _uz, _fz, _34;
      float        _px, _py, _pz, _44;
  //  };
} _Matrix;

struct _MATH_DESCRIPTION
{
  char   *Name;
  short   ID;
};


#endif