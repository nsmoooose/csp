/* Copyright (C) Dante Treglia II and Mark A. DeLoura, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II and Mark A. DeLoura, 2000"
 *
 * Modified for the CSP project by Wolverine69
 */
//==========================================================
// C++ Matrix Library
// Version: 2.6
// Date: May 19, 2000
// Authors: Dante Treglia II and Mark A. DeLoura
// Thanks to: Miguel Gomez, Stan Melax, Pete Isensee, 
//   Gabor Nagy, Scott Bilas, James Boer, Eric Lengyel
//==========================================================
#include "mtxlib.h"
#include <cmath>
#include <cassert>

////////////////////////////////////////////////////////////
// Miscellaneous vector functions
//

// Return Normal of csp_vector2's
csp_vector2 Normalized(const csp_vector2 &a)
{
  csp_vector2 ret(a);
  return ret.normalize();
}
// Return Normal of csp_vector3's
csp_vector3 Normalized(const csp_vector3 &a)
{
  csp_vector3 ret(a);
  return ret.normalize();
}
// Return Normal of csp_vector4's
csp_vector4 Normalized(const csp_vector4 &a)
{
  csp_vector4 ret(a);
  return ret.normalize();
}

// Dot product of two csp_vector2's
csp_scalar DotProduct(const csp_vector2 &a, const csp_vector2 &b) 
{
  return a.x*b.x + a.y*b.y;
}

// Dot product of two csp_vector3's
csp_scalar DotProduct(const csp_vector3 &a, const csp_vector3 &b) 
{
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

// Dot product of two csp_vector4's
csp_scalar DotProduct(const csp_vector4 &a, const csp_vector4 &b) 
{
  return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

// Swap two csp_vector2's
void SwapVec(csp_vector2 &a, csp_vector2 &b) 
{
  csp_vector2 tmp(a);

  a = b;
  b = tmp;
}

// Swap two csp_vector3's
void SwapVec(csp_vector3 &a, csp_vector3 &b) 
{
  csp_vector3 tmp(a);

  a = b;
  b = tmp;
}

// Swap two csp_vector4's
void SwapVec(csp_vector4 &a, csp_vector4 &b) 
{
  csp_vector4 tmp(a);

  a = b;
  b = tmp;
}

// Cross product of two csp_vector3's
csp_vector3 CrossProduct(const csp_vector3 &a, const csp_vector3 &b) 
{
  return csp_vector3(a.y*b.z - a.z*b.y,
      a.z*b.x - a.x*b.z,
      a.x*b.y - a.y*b.x);
}

// Are these two csp_vector2's nearly equal?
bool NearlyEquals( const csp_vector2& a, const csp_vector2& b, csp_scalar r ) 
{
  csp_vector2 diff = a - b;  // difference

  return(DotProduct(diff, diff) < r*r);  // radius
}

// Are these two csp_vector3's nearly equal?
bool NearlyEquals( const csp_vector3& a, const csp_vector3& b, csp_scalar r ) 
{
  csp_vector3 diff = a - b;  // difference

  return(DotProduct(diff, diff) < r*r);  // radius
}

// Are these two csp_vector4's nearly equal?
bool NearlyEquals( const csp_vector4& a, const csp_vector4& b, csp_scalar r ) 
{
  csp_vector4 diff = a - b;  // difference

  return(DotProduct(diff, diff) < r*r);  // radius
}


////////////////////////////////////////////////////////////
// csp_matrix33 class
//

// Multiply the csp_matrix33 by another csp_matrix33
csp_matrix33 &csp_matrix33::operator *= (const csp_matrix33 &m) 
{
  csp_matrix33 t;

  for (unsigned int r = 0; r < 3; r++)
  {
  for (unsigned int c = 0; c < 3; c++)
  {
    csp_scalar f = 0;

    f += col[0][r] * m[c][0];
    f += col[1][r] * m[c][1];
    f += col[2][r] * m[c][2];

    t[c][r] = f;
  }
  }

  *this = t;

  return *this;
}

// Transpose the csp_matrix33
csp_matrix33 &csp_matrix33::transpose() 
{
  csp_scalar t;

  for (unsigned int c = 0; c < 3; c++)
  {
  for (unsigned int r = c + 1; r < 3; r++)
  {
    t = col[c][r];
    col[c][r] = col[r][c];
    col[r][c] = t;
  } 
  }

  return *this;
}

// Invert the csp_matrix33
csp_matrix33 &csp_matrix33::invert() 
{
  csp_matrix33 a(*this);
  csp_matrix33 b(IdentityCsp_Matrix33());

  unsigned int c, r;
  unsigned int cc;
  unsigned int rowMax; // Points to max abs value row in this column
  unsigned int row;
  csp_scalar tmp;

  // Go through columns
  for (c=0; c<3; c++)
  {
  // Find the row with max value in this column
  rowMax = c;
  for (r=c+1; r<3; r++)
  {
    if (fabs(a[c][r]) > fabs(a[c][rowMax]))
    {
    rowMax = r;
    }
  }

  // If the max value here is 0, we can't invert.  Return identity.
  if (a[rowMax][c] == 0.0F)
    return(identity());

  // Swap row "rowMax" with row "c"
  for (cc=0; cc<3; cc++)
  {
    tmp = a[cc][c];
    a[cc][c] = a[cc][rowMax];
    a[cc][rowMax] = tmp;
    tmp = b[cc][c];
    b[cc][c] = b[cc][rowMax];
    b[cc][rowMax] = tmp;
  }

  // Now everything we do is on row "c".
  // Set the max cell to 1 by dividing the entire row by that value
  tmp = a[c][c];
  for (cc=0; cc<3; cc++)
  {
    a[cc][c] /= tmp;
    b[cc][c] /= tmp;
  }

  // Now do the other rows, so that this column only has a 1 and 0's
  for (row = 0; row < 3; row++)
  {
    if (row != c)
    {
    tmp = a[c][row];
    for (cc=0; cc<3; cc++)
    {
      a[cc][row] -= a[cc][c] * tmp;
      b[cc][row] -= b[cc][c] * tmp;
    }
    }
  }

  }

  *this = b;

  return *this;
}

// Return a csp_matrix33 set to the identity csp_matrix
csp_matrix33 IdentityMatrix33() 
{
  csp_matrix33 ret;

  return ret.identity();
}

// Return the transpose of the csp_matrix33
csp_matrix33 TransposeMatrix33(const csp_matrix33 &m) 
{
  csp_matrix33 ret(m);

  return ret.transpose();
}

// Return the inverted csp_matrix33
csp_matrix33 InvertMatrix33(const csp_matrix33 &m) 
{
  csp_matrix33 ret(m);

  return ret.invert();
}

// Return a 2D rotation csp_matrix33
csp_matrix33 RotateRadMatrix33(csp_scalar rad) 
{
  csp_matrix33 ret;
  csp_scalar sinA, cosA;

  sinA = (csp_scalar)sin(rad);
  cosA = (csp_scalar)cos(rad);

  ret[0][0] = cosA; ret[1][0] = -sinA; ret[2][0] = 0.0F;
  ret[0][1] = sinA; ret[1][1] =  cosA; ret[2][1] = 0.0F;
  ret[0][2] = 0.0F; ret[1][2] =  0.0F; ret[2][2] = 1.0F;

  return ret;
}

// Return a 2D translation csp_matrix33
csp_matrix33 TranslateMatrix33(csp_scalar x, csp_scalar y) 
{
  csp_matrix33 ret;

  ret.identity();
  ret[2][0] = x;
  ret[2][1] = y;

  return ret;
}

// Return a 2D/3D scale csp_matrix33
csp_matrix33 ScaleMatrix33(csp_scalar x, csp_scalar y, csp_scalar z) 
{
  csp_matrix33 ret;

  ret.identity();
  ret[0][0] = x;
  ret[1][1] = y;
  ret[2][2] = z;

  return ret;
}


////////////////////////////////////////////////////////////
// csp_matrix44 class
//

// Multiply the csp_matrix44 by another csp_matrix44
csp_matrix44 &csp_matrix44::operator *= (const csp_matrix44 &m) 
{
  csp_matrix44 t;
  for (unsigned int r = 0; r < 4; r++)
  {
  for (unsigned int c = 0; c < 4; c++)
  {
    csp_scalar f = 0;

    f += (col[0][r] * m[c][0]);
    f += (col[1][r] * m[c][1]);
    f += (col[2][r] * m[c][2]);
    f += (col[3][r] * m[c][3]);

    t[c][r] = f;
  }
  }
  *this = t;
  return *this;
}


// Transpose the csp_matrix44
csp_matrix44 &csp_matrix44::transpose() 
{
  csp_scalar t;

  for (unsigned int c = 0; c < 4; c++)
  {
  for (unsigned int r = c + 1; r < 4; r++)
  {
    t = col[c][r];
    col[c][r] = col[r][c];
    col[r][c] = t;
  } 
  } 

  return *this;
}

// Invert the csp_matrix44
csp_matrix44 &csp_matrix44::invert() 
{
  csp_matrix44 a(*this);
  csp_matrix44 b(IdentityCsp_Matrix44());

  unsigned int r, c;
  unsigned int cc;
  unsigned int rowMax; // Points to max abs value row in this column
  unsigned int row;
  csp_scalar tmp;

  // Go through columns
  for (c=0; c<4; c++)
  {

  // Find the row with max value in this column
  rowMax = c;
  for (r=c+1; r<4; r++)
  {
    if (fabs(a[c][r]) > fabs(a[c][rowMax]))
    {
    rowMax = r;
    }
  }

  // If the max value here is 0, we can't invert.  Return identity.
  if (a[rowMax][c] == 0.0F)
    return(identity());

  // Swap row "rowMax" with row "c"
  for (cc=0; cc<4; cc++)
  {
    tmp = a[cc][c];
    a[cc][c] = a[cc][rowMax];
    a[cc][rowMax] = tmp;
    tmp = b[cc][c];
    b[cc][c] = b[cc][rowMax];
    b[cc][rowMax] = tmp;
  }

  // Now everything we do is on row "c".
  // Set the max cell to 1 by dividing the entire row by that value
  tmp = a[c][c];
  for (cc=0; cc<4; cc++)
  {
    a[cc][c] /= tmp;
    b[cc][c] /= tmp;
  }

  // Now do the other rows, so that this column only has a 1 and 0's
  for (row = 0; row < 4; row++)
  {
    if (row != c)
    {
    tmp = a[c][row];
    for (cc=0; cc<4; cc++)
    {
      a[cc][row] -= a[cc][c] * tmp;
      b[cc][row] -= b[cc][c] * tmp;
    }
    }
  }

  }

  *this = b;

  return *this;
}

// Return a csp_matrix44 set to the identity csp_matrix
csp_matrix44 IdentityMatrix44() 
{
  csp_matrix44 ret;

  return ret.identity();
}

// Return the transpose of the csp_matrix44
csp_matrix44 TransposeMatrix44(const csp_matrix44 &m) 
{
  csp_matrix44 ret(m);

  return ret.transpose();
}

// Return the inverted csp_matrix44
csp_matrix44 InvertCsp_Matrix44(const csp_matrix44 &m) 
{
  csp_matrix44 ret(m);

  return ret.invert();
}

// Return a 3D axis-rotation csp_matrix44
// Pass in 'x', 'y', or 'z' for the axis.
csp_matrix44 RotateRadMatrix44(char axis, csp_scalar rad) 
{
  csp_matrix44 ret;
  csp_scalar sinA, cosA;

  sinA = (csp_scalar)sin(rad);
  cosA = (csp_scalar)cos(rad);

  switch (axis)
  {
  case 'x':
  case 'X':
    ret[0][0] =  1.0F; ret[1][0] =  0.0F; ret[2][0] =  0.0F;
    ret[0][1] =  0.0F; ret[1][1] =  cosA; ret[2][1] = -sinA;
    ret[0][2] =  0.0F; ret[1][2] =  sinA; ret[2][2] =  cosA;
    break;

  case 'y':
  case 'Y':
    ret[0][0] =  cosA; ret[1][0] =  0.0F; ret[2][0] =  sinA;
    ret[0][1] =  0.0F; ret[1][1] =  1.0F; ret[2][1] =  0.0F;
    ret[0][2] = -sinA; ret[1][2] =  0.0F; ret[2][2] =  cosA;
    break;

  case 'z':
  case 'Z':
    ret[0][0] =  cosA; ret[1][0] = -sinA; ret[2][0] =  0.0F;
    ret[0][1] =  sinA; ret[1][1] =  cosA; ret[2][1] =  0.0F;
    ret[0][2] =  0.0F; ret[1][2] =  0.0F; ret[2][2] =  1.0F;
    break;
  }

  ret[0][3] = 0.0F; ret[1][3] = 0.0F; ret[2][3] = 0.0F;
  ret[3][0] = 0.0F;
  ret[3][1] = 0.0F;
  ret[3][2] = 0.0F;
  ret[3][3] = 1.0F;

  return ret;
}

// Return a 3D axis-rotation csp_matrix44
// Pass in an arbitrary csp_vector3 axis.
csp_matrix44 RotateRadMatrix44(const csp_vector3 &axis, csp_scalar rad) 
{
  csp_matrix44 ret;
  csp_scalar sinA, cosA;
  csp_scalar invCosA;
  csp_vector3 nrm = axis;
  csp_scalar x, y, z;
  csp_scalar xSq, ySq, zSq;

  nrm.normalize();
  sinA = (csp_scalar)sin(rad);
  cosA = (csp_scalar)cos(rad);
  invCosA = 1.0F - cosA;

  x = nrm.x;
  y = nrm.y;
  z = nrm.z;

  xSq = x * x;
  ySq = y * y;
  zSq = z * z;

  ret[0][0] = (invCosA * xSq) + (cosA);
  ret[1][0] = (invCosA * x * y) - (sinA * z );
  ret[2][0] = (invCosA * x * z) + (sinA * y );
  ret[3][0] = 0.0F;

  ret[0][1] = (invCosA * x * y) + (sinA * z);
  ret[1][1] = (invCosA * ySq) + (cosA);
  ret[2][1] = (invCosA * y * z) - (sinA * x);
  ret[3][1] = 0.0F;

  ret[0][2] = (invCosA * x * z) - (sinA * y);
  ret[1][2] = (invCosA * y * z) + (sinA * x);
  ret[2][2] = (invCosA * zSq) + (cosA);
  ret[3][2] = 0.0F;

  ret[0][3] = 0.0F;
  ret[1][3] = 0.0F;
  ret[2][3] = 0.0F;
  ret[3][3] = 1.0F;

  return ret;
}

// Return a 3D translation csp_matrix44
csp_matrix44 TranslateMatrix44(csp_scalar x, csp_scalar y, csp_scalar z) 
{
  csp_matrix44 ret;

  ret.identity();
  ret[3][0] = x;
  ret[3][1] = y;
  ret[3][2] = z;

  return ret;
}

// Return a 3D/4D scale csp_matrix44
csp_matrix44 ScaleMatrix44(csp_scalar x, csp_scalar y, csp_scalar z, csp_scalar w) 
{
  csp_matrix44 ret;

  ret.identity();
  ret[0][0] = x;
  ret[1][1] = y;
  ret[2][2] = z;
  ret[3][3] = w;

  return ret;
}

// Return a "lookat" csp_matrix44 given the current camera position (csp_vector3),
//   camera-up csp_vector3, and camera-target csp_vector3.
csp_matrix44 LookAtMatrix44(const csp_vector3 &camPos, const csp_vector3 &target, 
    const csp_vector3 &camUp ) 
{
  csp_matrix44 ret;

  csp_vector3 F = target - camPos;
  F.normalize();

  csp_vector3 S = CrossProduct(F, Normalized(camUp));
  S.normalize();

  csp_vector3 U = CrossProduct(S, F);
  U.normalize();

  ret[0][0] = S.x;
  ret[1][0] = S.y;
  ret[2][0] = S.z;
  ret[3][0] = 0.0;

  ret[0][1] = U.x;
  ret[1][1] = U.y;
  ret[2][1] = U.z;
  ret[3][1] = 0.0;

  ret[0][2] = -F.x;
  ret[1][2] = -F.y;
  ret[2][2] = -F.z;
  ret[3][2] = 0.0;

  ret[0][3] = 0.0F;
  ret[1][3] = 0.0F;
  ret[2][3] = 0.0F;
  ret[3][3] = 1.0F;

  ret *= TranslateMatrix44(-camPos.x, -camPos.y, -camPos.z);

  return ret;
}

// Return a frustum csp_matrix44 given the left, right, bottom, top,
//   near, and far values for the frustum boundaries.
csp_matrix44 FrustumMatrix44(csp_scalar l, csp_scalar r, 
    csp_scalar b, csp_scalar t, csp_scalar n, csp_scalar f) 
{
  csp_matrix44 ret;
  csp_scalar width = r-l;
  csp_scalar height = t-b;
  csp_scalar depth = f-n;

  ret[0][0] = (2*n) / width;
  ret[0][1] = 0.0F;
  ret[0][2] = 0.0F;
  ret[0][3] = 0.0F;

  ret[1][0] = 0.0F;
  ret[1][1] = (2*n) / height;
  ret[1][2] = 0.0F;
  ret[1][3] = 0.0F;

  ret[2][0] = (r + l) / width;
  ret[2][1] = (t + b) / height;
  ret[2][2] = -(f + n) / depth;
  ret[2][3] = -1.0F;

  ret[3][0] = 0.0F;
  ret[3][1] = 0.0F;
  ret[3][2] = -(2*f*n) / depth;
  ret[3][3] = 0.0F;

  return ret;
}

// Return a perspective csp_matrix44 given the field-of-view in the Y
//   direction in degrees, the aspect ratio of Y/X, and near and
//   far plane distances.
csp_matrix44 PerspectiveMatrix44(csp_scalar fovY, csp_scalar aspect, csp_scalar n, csp_scalar f) 
{
  csp_matrix44 ret;
  csp_scalar angle;
  csp_scalar cot;

  angle = fovY / 2.0F;
  angle = DegToRad( angle );

  cot = (csp_scalar) cos(angle) / (csp_scalar) sin(angle);

  ret[0][0] = cot / aspect;
  ret[0][1] = 0.0F;
  ret[0][2] = 0.0F;
  ret[0][3] = 0.0F;

  ret[1][0] = 0.0F;
  ret[1][1] = cot;
  ret[1][2] = 0.0F;
  ret[1][3] = 0.0F;

  ret[2][0] = 0.0F;
  ret[2][1] = 0.0F;
  ret[2][2] = -(f + n) / (f - n);
  ret[2][3] = -1.0F;


  ret[3][0] = 0.0F;
  ret[3][1] = 0.0F;
  ret[3][2] = -(2*f*n) / (f - n);
  ret[3][3] = 0.0F;

  return ret;
}

// Return an orthographic csp_matrix44 given the left, right, bottom, top,
//   near, and far values for the frustum boundaries.
csp_matrix44 OrthoMatrix44(csp_scalar l, csp_scalar r, 
    csp_scalar b, csp_scalar t, csp_scalar n, csp_scalar f) 
{
  csp_matrix44 ret;
  csp_scalar width = r-l;
  csp_scalar height = t-b;
  csp_scalar depth = f-n;

  ret[0][0] = 2.0F / width;
  ret[0][1] = 0.0F;
  ret[0][2] = 0.0F;
  ret[0][3] = 0.0F;

  ret[1][0] = 0.0F;
  ret[1][1] = 2.0F / height;
  ret[1][2] = 0.0F;
  ret[1][3] = 0.0F;

  ret[2][0] = 0.0F;
  ret[2][1] = 0.0F;
  ret[2][2] = -(2.0F) / depth;
  ret[2][3] = 0.0F;

  ret[3][0] = -(r + l) / width;
  ret[1][3] = -(t + b) / height;
  ret[3][2] = -(f + n) / depth;
  ret[3][3] = 1.0F;

  return ret;
}

// Return an orientation csp_matrix using 3 basis normalized csp_vectors
matrix44    OrthoNormalMatrix44(const csp_vector3 &xdir, 
    const csp_vector3 &ydir, const csp_vector3 &zdir)
{
  csp_matrix44 ret;

  ret[0] = (csp_vector4)xdir;
  ret[1] = (csp_vector4)ydir;
  ret[2] = (csp_vector4)zdir;
  ret[3].set(0.0, 0.0, 0.0, 1.0);

  return ret;
}


////////////////////////////////////////////////////////////
// Debug functions
//

// Print a csp_vector2 to a file
void csp_vector2::fprint(FILE* file, char* str) const 
{
  fprintf(file, "%scsp_vector2: <%f, %f>\n", str, x, y);
}

// Print a csp_vector3 to a file
void csp_vector3::fprint(FILE* file, char* str) const 
{
  fprintf(file, "%scsp_vector3: <%f, %f, %f>\n", str, x, y, z);
}

// Print a csp_vector4 to a file
void csp_vector4::fprint(FILE* file, char* str) const 
{
  fprintf(file, "%scsp_vector4: <%f, %f, %f, %f>\n", str, x, y, z, w);
}

// Print a csp_matrix33 to a file
void csp_matrix33::fprint(FILE* file, char * str) const 
{
  fprintf(file, "%scsp_matrix33:\n", str);
  csp_vector3 row0(col[0][0], col[1][0], col[2][0]);
  row0.fprint(file, "\t");
  csp_vector3 row1(col[0][1], col[1][1], col[2][1]);
  row1.fprint(file, "\t");
  csp_vector3 row2(col[0][2], col[1][2], col[2][2]);
  row2.fprint(file, "\t");
}

// Print a csp_matrix44 to a file
void csp_matrix44::fprint(FILE* file, char* str) const 
{
  fprintf(file, "%scsp_matrix44:\n", str);
  csp_vector4 row0(col[0][0], col[1][0], col[2][0], col[3][0]);
  row0.fprint(file, "\t");
  csp_vector4 row1(col[0][1], col[1][1], col[2][1], col[3][1]);
  row1.fprint(file, "\t");
  csp_vector4 row2(col[0][2], col[1][2], col[2][2], col[3][2]);
  row2.fprint(file, "\t");
  csp_vector4 row3(col[0][3], col[1][3], col[2][3], col[3][3]);
  row3.fprint(file, "\t");
}
