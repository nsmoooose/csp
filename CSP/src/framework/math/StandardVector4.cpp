#include "StandardVector4.h"

_Vector4 & _Vector4::Normalize()
{
  float m = Length();
  if (m>0.0f)
    m = 1.0f/m;
  else m = 0.0f;
  x *= m;
  y *= m;
  z *= m;
  w *= m;
  return *this;
}
