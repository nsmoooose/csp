// A Generic Framework Math Library.

#ifndef SMTag

  #define SMTag

  #include "TypesMath.h"

  class StandardMath
  {

  private:

  public:

      virtual void    MatrixIdentity(_Matrix *matrix) = 0;
      virtual void    MatrixTranslation(_Matrix *matrix, float x, float y, float z) = 0;
      virtual void    MatrixRotationY(_Matrix *matrix, float y) = 0;
      virtual void    MatrixRotationX(_Matrix *matrix, float x) = 0;
      virtual void    MatrixRotationZ(_Matrix *matrix, float z) = 0;
      virtual void    MatrixPerspectiveFOV(_Matrix *matrix, short FOV, short Width, short Height, float NearZ, float FarZ) = 0;
      virtual void    MatrixMultiply(_Matrix *matrix, _Matrix *matrixsource) = 0;
      virtual void    MatrixMultiply2(_Matrix *matrix, _Matrix *matrixsource, _Matrix *matrixsource2) = 0;
      virtual void    MatrixInverse(_Matrix *matrix) = 0;
      virtual void    MatrixInverse2(_Matrix *matrix, _Matrix *source) = 0;
      virtual void    MatrixScale(_Matrix *matrix, float x, float y, float z) = 0;

      virtual void    SetVector(_Vector *vector, float x, float y, float z) = 0;
      virtual _Vector CalculateNormal(_Vector *Base, _Vector *Left, _Vector *Right) = 0;
      virtual _Vector CrossProduct(_Vector *Forward, _Vector *Up) = 0;

      virtual float   ToDegree(float radian) = 0;
      virtual float   ToRadian(float degree) = 0;

  };

#endif

