#include "StandardMath.h"

#include <d3dx8.h>

class DirectMathClass: public StandardMath
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

inline void DirectMathClass::MatrixIdentity(_Matrix *matrix)
{
  D3DXMatrixIdentity((D3DXMATRIX *)matrix);
}

inline void DirectMathClass::MatrixTranslation(_Matrix *matrix, float x, float y, float z)
{
  D3DXMatrixTranslation((D3DXMATRIX *)matrix, x, y, z);
}

inline void DirectMathClass::MatrixScale(_Matrix *matrix, float x, float y, float z)
{
  D3DXMatrixScaling((D3DXMATRIX *)matrix, x, y, z);
}

inline void DirectMathClass::MatrixRotationY(_Matrix *matrix, float y)
{
  D3DXMatrixRotationY((D3DXMATRIX *)matrix, y);
}

inline void DirectMathClass::MatrixRotationX(_Matrix *matrix, float x)
{
  D3DXMatrixRotationX((D3DXMATRIX *)matrix, x);
}

inline void DirectMathClass::MatrixRotationZ(_Matrix *matrix, float z)
{
  D3DXMatrixRotationZ((D3DXMATRIX *)matrix, z);
}

inline void DirectMathClass::MatrixMultiply(_Matrix *matrix, _Matrix *matrixsource)
{
  D3DXMatrixMultiply((D3DXMATRIX *)matrix, (D3DXMATRIX *)matrix, (D3DXMATRIX *)matrixsource);
}

inline void DirectMathClass::MatrixMultiply2(_Matrix *matrix, _Matrix *matrixsource, _Matrix *matrixsource2)
{
  D3DXMatrixMultiply((D3DXMATRIX *)matrix, (D3DXMATRIX *)matrixsource, (D3DXMATRIX *)matrixsource2 );
}

inline void DirectMathClass::MatrixPerspectiveFOV(_Matrix *matrix, short FOV, short Width, short Height, float NearZ, float FarZ)
{
  D3DXMatrixPerspectiveFovLH((D3DXMATRIX *)matrix, D3DXToRadian(FOV / 2), (float)Width / (float)Height, NearZ, FarZ);
}

inline void DirectMathClass::SetVector(_Vector *vector, float x, float y, float z)
{
  ((D3DXVECTOR3 *)vector)->x = x;
  ((D3DXVECTOR3 *)vector)->y = y;
  ((D3DXVECTOR3 *)vector)->z = z;

  return;
}

inline void DirectMathClass::MatrixInverse(_Matrix *matrix)
{
  D3DXMatrixInverse((D3DXMATRIX *)matrix, 0, (D3DXMATRIX *)matrix);
}

inline void DirectMathClass::MatrixInverse2(_Matrix *matrix, _Matrix *source)
{
  D3DXMatrixInverse((D3DXMATRIX *)matrix, 0, (D3DXMATRIX *)source);
}

inline _Vector DirectMathClass::CalculateNormal(_Vector *Base, _Vector *Left, _Vector *Right)
{

  _Vector vec1, vec2, vecr;

  vec1.x = Left->x - Base->x;
  vec1.y = Left->y - Base->y;
  vec1.z = Left->z - Base->z;

  vec2.x = Right->x - Base->x;
  vec2.y = Right->y - Base->y;
  vec2.z = Right->z - Base->z;

  D3DXVec3Cross((D3DXVECTOR3 *)&vecr, (D3DXVECTOR3 *)&vec1, (D3DXVECTOR3 *)&vec2);
  D3DXVec3Normalize((D3DXVECTOR3 *)&vecr, (D3DXVECTOR3 *)&vecr);

  return vecr;
}

inline _Vector DirectMathClass::CrossProduct(_Vector *Forward, _Vector *Up)
{

  _Vector vecr;

  D3DXVec3Cross((D3DXVECTOR3 *)&vecr, (D3DXVECTOR3 *)Forward, (D3DXVECTOR3 *)Up);
  D3DXVec3Normalize((D3DXVECTOR3 *)&vecr, (D3DXVECTOR3 *)&vecr);

  return vecr;
}

inline float DirectMathClass::ToDegree(float radian)
{
  return D3DXToDegree(radian);
}

inline float DirectMathClass::ToRadian(float degree)
{
  return D3DXToRadian(degree);
}