#include "StandardMath.h"

class SimpleMath: public StandardMath
{
  
 private:
  
 public:
  void MatrixIdentity(_Matrix *matrix);
  void MatrixTranslation(_Matrix *matrix, float x, float y, float z);
  void MatrixRotationY(_Matrix *matrix, float y);
  void MatrixRotationX(_Matrix *matrix, float x);
  void MatrixRotationZ(_Matrix *matrix, float z);
  void MatrixPerspectiveFOV(_Matrix *matrix, short FOV, short Width, short Height, float NearZ, float FarZ);
  void MatrixMultiply(_Matrix *matrix, _Matrix *matrixsource);
  void MatrixMultiply2(_Matrix *matrix, _Matrix *matrixsource, _Matrix *matrixsource2);
  void MatrixInverse(_Matrix *matrix);
  void MatrixInverse2(_Matrix *matrix, _Matrix *source);
  void MatrixScale(_Matrix *matrix, float x, float y, float z);
  
  void SetVector(_Vector *vector, float x, float y, float z);
  _Vector CalculateNormal(_Vector *Base, _Vector *Left, _Vector *Right);
  _Vector CrossProduct(_Vector *Forward, _Vector *Up);
  
  float ToDegree(float radian);
  float ToRadian(float degree);
};

inline void SimpleMath::MatrixIdentity(_Matrix *matrix)
{
  memset(matrix,0,sizeof(*matrix));
  matrix->_rx=1.0f;
  matrix->_uy=1.0f;
  matrix->_fz=1.0f;
  matrix->_44=1.0f;
}

inline void SimpleMath::MatrixTranslation(_Matrix *matrix, float x, float y, float z)
{
  
}

inline void SimpleMath::MatrixScale(_Matrix *matrix, float x, float y, float z)
{
  
}

inline void SimpleMath::MatrixRotationY(_Matrix *matrix, float y)
{
  
}

inline void SimpleMath::MatrixRotationX(_Matrix *matrix, float x)
{
  
}

inline void SimpleMath::MatrixRotationZ(_Matrix *matrix, float z)
{
  
}

inline void SimpleMath::MatrixMultiply(_Matrix *matrix, _Matrix *matrixsource)
{
  
}

inline void SimpleMath::MatrixMultiply2(_Matrix *matrix, _Matrix *matrixsource, _Matrix *matrixsource2)
{
  
}

inline void SimpleMath::MatrixPerspectiveFOV(_Matrix *matrix, short FOV, short Width, short Height, float NearZ, float FarZ)
{
  
}

inline void SimpleMath::SetVector(_Vector *vector, float x, float y, float z)
{
  vector->x = x;
  vector->y = y;
  vector->z = z;
}

inline void SimpleMath::MatrixInverse(_Matrix *matrix)
{

}

inline void SimpleMath::MatrixInverse2(_Matrix *matrix, _Matrix *source)
{

}

inline _Vector SimpleMath::CalculateNormal(_Vector *Base, _Vector *Left, _Vector *Right)
{

  _Vector vec1, vec2, vecr;

  vec1.x = Left->x - Base->x;
  vec1.y = Left->y - Base->y;
  vec1.z = Left->z - Base->z;

  vec2.x = Right->x - Base->x;
  vec2.y = Right->y - Base->y;
  vec2.z = Right->z - Base->z;

  return vecr;
}

inline _Vector SimpleMath::CrossProduct(_Vector *Forward, _Vector *Up)
{

  _Vector vecr;

  return vecr;
}

inline float SimpleMath::ToDegree(float radian)
{
  return 0;
}

inline float SimpleMath::ToRadian(float degree)
{
  return 0;
}
