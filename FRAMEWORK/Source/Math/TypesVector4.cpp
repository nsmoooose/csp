// Initial CSP version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.

#include "Framework.h"

//namespace Math 
//{

StandardVector4 & StandardVector4::Normalize()
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

void StandardVector4::Print(FILE * stream)
{
  fprintf(stream, "[%f, %f, %f %f]\n", x, y, z, w);
}


//}
