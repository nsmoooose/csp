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

#ifndef _MTXLIB_H
#define _MTXLIB_H
#include <cstdio>
#include <cmath>
#include <cassert>

static inline csp_scalar DegToRad(csp_scalar a) { return a*0.01745329252f;};
static inline csp_scalar RadToDeg(csp_scalar a) { return a*57.29577951f;};

typedef float csp_scalar;

class csp_vector2;
class csp_vector3;
class csp_vector4;
class csp_matrix33;
class csp_matrix44;
class csp_rect;

////////////////////////////////////////////////////////////
// csp_vector2 class
//

class csp_vector2 
{
public:
  // Members
  csp_scalar x, y;

public:
  // Constructors
  csp_vector2() {};
  // Constructor with initializing csp_scalar values
  csp_vector2(csp_float inX, csp_float inY): x(inX), y(inY) {}
  // Constructor with initializing vector2
  csp_vector2(const csp_vector2 &v): x(v.x), y(v.y) {}

public:
  // Array indexing
  csp_scalar                 &operator [] (unsigned int i) {
    assert (i<2);
    return *(&x+i);
  }
  // Array indexing
  const csp_float           &operator [] (unsigned int i) const {
    assert (i<2);
    return *(&x+i);
  }
  // Add a csp_vector2 to this one
  csp_vector2               &operator += (const csp_vector2 &v) {
    x += v.x;
    y += v.y;
    return *this;
  }
  // Subtract a csp_vector2 from this one
  csp_vector2               &operator -= (const csp_vector2 &v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }
  // Multiply the csp_vector2 by a csp_scalar
  csp_vector2               &operator *= (csp_scalar f) {
    x *= f;
    y *= f;
    return *this;
  }
  // Divide the csp_vector2 by a csp_scalar
  csp_vector2               &operator /= (csp_scalar f) {
    x /= f;
    y /= f;
    return *this;
  }
  // Are these two csp_vector2's equal?
  friend bool           operator == (const csp_vector2 &a, const csp_vector2 &b) {
    return((a.x == b.x) && (a.y == b.y));
  }
  // Are these two csp_vector2's not equal?
  friend bool           operator != (const csp_vector2 &a, const csp_vector2 &b) {
    return((a.x != b.x) || (a.y != b.y));
  }
  // Negate this csp_vector
  friend csp_vector2        operator - (const csp_vector2 &a) {
    return csp_vector2(-a.x, -a.y);
  }
  // Add two csp_vector2's
  friend csp_vector2        operator + (const csp_vector2 &a, const csp_vector2 &b) {
    csp_vector2 ret(a);
    ret += b;
    return ret;
  }
  // Subtract one csp_vector2 from another
  friend csp_vector2        operator - (const csp_vector2 &a, const csp_vector2 &b) {
    csp_vector2 ret(a);
    ret -= b;
    return ret;
  }
  // Multiply csp_vector2 by a csp_scalar
  friend csp_vector2        operator * (const csp_vector2 &v, csp_scalar f) {
    return csp_vector2(f * v.x, f * v.y);
  }
  // Multiply csp_vector2 by a csp_scalar
  friend csp_vector2        operator * (csp_scalar f, const csp_vector2 &v) {
    return csp_vector2(f * v.x, f * v.y);
  }
  // Divide csp_vector2 by a csp_scalar
  friend csp_vector2        operator / (const csp_vector2 &v, csp_scalar f) {
    return csp_vector2(v.x / f, v.y / f);
  }

public:
  // Methods
  // Set Values
  void                  set(csp_scalar xIn, csp_scalar yIn) {
    x = xIn;
    y = yIn;
  }
  // Get length of a csp_vector2
  csp_scalar                 length() const {
    return(csp_scalar) sqrt(x*x + y*y);
  }
  // Get squared length of a csp_vector2
  csp_scalar                 lengthSqr() const {
    return(x*x + y*y);
  }
  // Does csp_vector2 equal (0, 0)?
  bool                  isZero() const {
    return((x == 0.0F) && (y == 0.0F));
  }
  // Normalize a csp_vector2
  csp_vector2               &normalize() {
    csp_scalar m = length();

    if (m > 0.0F)
      m = 1.0F / m;
    else
      m = 0.0F;
    x *= m;
    y *= m;

    return *this;
  }

  // Debug
  void                  fprint(FILE* file, char* str) const;
};


////////////////////////////////////////////////////////////
// csp_vector3 class
//

class csp_vector3 
{
public:
  // Members
  csp_scalar x, y, z;

public:
  // Constructors
  csp_vector3() {};
  // Constructor with initializing csp_scalar values
  csp_vector3(csp_scalar inX, csp_scalar inY, csp_scalar inZ): x(inX), y(inY), z(inZ) {}
  // Constructor with initializing csp_vector3
  csp_vector3(const csp_vector3 &v): x(v.x), y(v.y), z(v.z) {}
  // Constructor with initializing csp_vector2
  explicit csp_vector3(const csp_vector2 &v): x(v.x), y(v.y), z(0.0F) {}
  // Constructor with initializing csp_vector4
  // TODO
  explicit csp_vector3(const csp_vector4 &v);

public:
  // Operators
  // Array indexing
  csp_scalar                 &operator [] (unsigned int i) {
    assert (i<3);
    return *(&x+i);
  }
  // Array indexing
  const csp_scalar           &operator [] (unsigned int i) const {
    assert (i<3);
    return *(&x+i);
  }
  // Assign from a csp_vector2
  csp_vector3               &operator =  (const csp_vector2 &v) {
    x = v.x;
    y = v.y;
    z = 0.0F;
    return *this;
  }
  // Add a csp_vector3 to this one
  csp_vector3               &operator += (const csp_vector3 &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }
  // Subtract a csp_vector3 from this one
  csp_vector3               &operator -= (const csp_vector3 &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }
  // Multiply the csp_vector3 by a csp_scalar
  csp_vector3               &operator *= (csp_scalar f) {
    x *= f;
    y *= f;
    z *= f;
    return *this;
  }
  // Divide the csp_vector3 by a csp_scalar
  csp_vector3               &operator /= (csp_scalar f) {
    x /= f;
    y /= f;
    z /= f;
    return *this;
  }
  // Are these two csp_vector3's equal?
  friend bool           operator == (const csp_vector3 &a, const csp_vector3 &b) {
    return((a.x == b.x) && (a.y == b.y) && (a.z == b.z));
  }
  // Are these two csp_vector3's not equal?
  friend bool           operator != (const csp_vector3 &a, const csp_vector3 &b) {
    return((a.x != b.x) || (a.y != b.y) || (a.z != b.z));
  }
  // Negate a csp_vector3
  friend csp_vector3        operator - (const csp_vector3 &a) {
    return csp_vector3(-a.x, -a.y, -a.z);
  }
  // Add two csp_vector3's
  friend csp_vector3        operator + (const csp_vector3 &a, const csp_vector3 &b) {
    csp_vector3 ret(a);
    ret += b;
    return ret;
  }
  // Subtract one csp_vector3 from another
  friend csp_vector3        operator - (const csp_vector3 &a, const csp_vector3 &b) {
    csp_vector3 ret(a);
    ret -= b;
    return ret;
  }
  // Multiply csp_vector3 by a csp_scalar
  friend csp_vector3        operator * (const csp_vector3 &v, csp_scalar f) {
    return csp_vector3(f * v.x, f * v.y, f * v.z);
  }
  // Multiply csp_vector3 by a csp_scalar
  friend csp_vector3        operator * (csp_scalar f, const csp_vector3 &v) {
    return csp_vector3(f * v.x, f * v.y, f * v.z);
  }
  // Divide csp_vector3 by a csp_scalar
  friend csp_vector3        operator / (const csp_vector3 &v, csp_scalar f) {
    return csp_vector3(v.x / f, v.y / f, v.z / f);
  }

public:
  // Methods
  // Set Values
  void                  set(csp_scalar xIn, csp_scalar yIn, csp_scalar zIn) {
    x = xIn;
    y = yIn;
    z = zIn;
  }
  // Get length of a csp_vector3
  csp_scalar                 length() const {
    return(csp_scalar) sqrt(x*x + y*y + z*z);
  }
  // Get squared length of a csp_vector3
  csp_scalar                 lengthSqr() const {
    return(x*x + y*y + z*z);
  }
  // Does csp_vector3 equal (0, 0, 0)?
  bool                  isZero() const {
    return((x == 0.0F) && (y == 0.0F) && (z == 0.0F));
  }
  // Normalize a csp_vector3
  csp_vector3               &normalize() {
    csp_scalar m = length();
    if (m > 0.0F)
      m = 1.0F / m;
    else
      m = 0.0F;
    x *= m;
    y *= m;
    z *= m;
    return *this;
  }

  // Debug
  void                  fprint(FILE* file, char* str) const;
};


////////////////////////////////////////////////////////////
// csp_vector4 class
//

class csp_vector4 
{
public:
  // Members
  csp_scalar x, y, z, w;

public:
  // Constructors
  // csp_vector4(): x(0), y(0), z(0), w(0) {};
  csp_vector4() {};
  // Constructor with initializing csp_scalar values
  csp_vector4(csp_scalar inX, csp_scalar inY, csp_scalar inZ, csp_scalar inW): x(inX), y(inY), z(inZ), w(inW) {};
  // Constructor with initializing csp_vector4
  csp_vector4(const csp_vector4 &v): x(v.x), y(v.y), z(v.z), w(v.w) {};
  // Constructor with initializing csp_vector3
  explicit csp_vector4(const csp_vector3 &v): x(v.x), y(v.y), z(v.z), w(0.0F) {};
  // Constructor with initializing csp_vector2
  explicit csp_vector4(const csp_vector2 &v): x(v.x), y(v.y), z(0.0F), w(0.0F) {};

public:
  // Operators
  // Array indexing
  csp_scalar                 &operator [] (unsigned int i) {
    assert (i<4);
    //return *(&x+i);
    return(i == 0) ? x : (i == 1) ? y : (i == 2) ? z : w;
  }
  // Array indexing
  const csp_scalar           &operator [] (unsigned int i) const {
    assert (i<4);
    //return *(&x+i);
    return(i == 0) ? x : (i == 1) ? y : (i == 2) ? z : w;
  }
  // Assign from a csp_vector3
  csp_vector4               &operator =  (const csp_vector3 &v) { 
    x = v.x;
    y = v.y;
    z = v.z;
    w = 0.0F;
    return *this;
  }
  // Assign from a csp_vector2
  csp_vector4               &operator =  (const csp_vector2 &v) {
    x = v.x;
    y = v.y;
    z = 0.0F;
    w = 0.0F;
    return *this;
  }
  // Add a csp_vector4 to this one
  csp_vector4               &operator += (const csp_vector4 &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
  }
  // Subtract a csp_vector4 from this one
  csp_vector4               &operator -= (const csp_vector4 &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
  }
  // Multiply the csp_vector4 by a csp_scalar
  csp_vector4               &operator *= (csp_scalar f) {
    x *= f;
    y *= f;
    z *= f;
    w *= f;
    return *this;
  }
  // Divide the csp_vector4 by a csp_scalar
  csp_vector4               &operator /= (csp_scalar f) {
    x /= f;
    y /= f;
    z /= f;
    w /= f;
    return *this;
  }
  // Are these two csp_vector4's equal?
  friend bool           operator == (const csp_vector4 &a, const csp_vector4 &b) {
    return((a.x == b.x) && (a.y == b.y) &&
        (a.z == b.z) && (a.w == b.w));
  }
  // Are these two csp_vector4's not equal?
  friend bool           operator != (const csp_vector4 &a, const csp_vector4 &b) {
    return((a.x != b.x) || (a.y != b.y) ||
        (a.z != b.z) || (a.w != b.w));
  }
  // Negate a csp_vector4
  friend csp_vector4        operator - (const csp_vector4 &a) {
    return csp_vector4(-a.x, -a.y, -a.z, -a.w);
  }
  // Add two csp_vector4's
  friend csp_vector4        operator + (const csp_vector4 &a, const csp_vector4 &b) {
    csp_vector4 ret(a);
    ret += b;
    return ret;
  }
  // Subtract one csp_vector4 from another
  friend csp_vector4        operator - (const csp_vector4 &a, const csp_vector4 &b) {
    csp_vector4 ret(a);
    ret -= b;
    return ret;
  }
  // Multiply csp_vector4 by a csp_scalar
  friend csp_vector4        operator * (const csp_vector4 &v, csp_scalar f) {
    return csp_vector4(f * v.x, f * v.y, f * v.z, f * v.w);
  }
  // Multiply csp_vector4 by a csp_scalar
  friend csp_vector4        operator * (csp_scalar f, const csp_vector4 &v) {
    return csp_vector4(f * v.x, f * v.y, f * v.z, f * v.w);
  }
  // Divide csp_vector4 by a csp_scalar
  friend csp_vector4        operator / (const csp_vector4 &v, csp_scalar f) {
    return csp_vector4(v.x / f, v.y / f, v.z / f, v.w / f);
  }

public:
  // Methods
  // Set Values
  void                  set(csp_scalar xIn, csp_scalar yIn, csp_scalar zIn, csp_scalar wIn) {
    x = xIn;
    y = yIn;
    z = zIn;
    w = wIn;
  }
  // Get length of a csp_vector4
  csp_scalar                 length() const {
    return(csp_scalar) sqrt(x*x + y*y + z*z + w*w);
  }
  // Get squared length of a csp_vector4
  csp_scalar                 lengthSqr() const {
    return(x*x + y*y + z*z + w*w);
  }
  // Does csp_vector4 equal (0, 0, 0, 0)?
  bool                  isZero() const {
    return((x == 0.0F) && (y == 0.0F) && (z == 0.0F) && (w == 0.0F));
  }
  // Normalize a csp_vector4
  csp_vector4               &normalize() {
    csp_scalar m = length();
    if (m > 0.0F)
      m = 1.0F / m;
    else
      m = 0.0F;
    x *= m;
    y *= m;
    z *= m;
    w *= m;
    return *this;
  }

  // Debug
  void                  fprint(FILE* file, char* str) const;
};


////////////////////////////////////////////////////////////
// Miscellaneous csp_vector functions
//

csp_vector2 Normalized(const csp_vector2 &a);
csp_vector3 Normalized(const csp_vector3 &a);
csp_vector4 Normalized(const csp_vector4 &a);
csp_scalar DotProduct(const csp_vector2 &a, const csp_vector2 &b);
csp_scalar DotProduct(const csp_vector3 &a, const csp_vector3 &b);
csp_scalar DotProduct(const csp_vector4 &a, const csp_vector4 &b);
void SwapVec(csp_vector2 &a, csp_vector2 &b);
void SwapVec(csp_vector3 &a, csp_vector3 &b);
void SwapVec(csp_vector4 &a, csp_vector4 &b);
csp_vector3 CrossProduct(const csp_vector3 &a, const csp_vector3 &b);
bool NearlyEquals(const csp_vector2 &a, const csp_vector2 &b, csp_scalar r);
bool NearlyEquals(const csp_vector3 &a, const csp_vector3 &b, csp_scalar r);
bool NearlyEquals(const csp_vector4 &a, const csp_vector4 &b, csp_scalar r);


////////////////////////////////////////////////////////////
// csp_matrix33 class
//

class csp_matrix33 
{
public:
  // Members
  csp_vector3       col[3];

public:
  // Constructors
  csp_matrix33() {};
  // Constructor with initializing value
  csp_matrix33(csp_scalar v) {
    col[0].set(v, v, v);
    col[1].set(v, v, v);
    col[2].set(v, v, v);
  }
  // Constructor with initializing csp_matrix33
  csp_matrix33(const csp_matrix33 &m) {
    col[0] = m[0];
    col[1] = m[1];
    col[2] = m[2];
  }
  // Constructor with initializing csp_vector3's
  csp_matrix33(const csp_vector3 &v0, const csp_vector3 &v1, const csp_vector3 &v2) {
    col[0] = v0;
    col[1] = v1;
    col[2] = v2;
  }

public:
  // Operators
  // Array indexing
  csp_vector3       &operator [] (unsigned int i) {
    assert (i<3);
    return(csp_vector3&)col[i];
  }
  // Array indexing
  const csp_vector3     &operator [] (unsigned int i) const {
    assert (i<3);
    return(csp_vector3&)col[i];
  }
  // Assign
  csp_matrix33      &operator =  (const csp_matrix33 &m) {
    col[0] = m[0];
    col[1] = m[1];
    col[2] = m[2];
    return *this;
  }
  // Add a csp_matrix33 to this one
  csp_matrix33      &operator += (const csp_matrix33 &m) {
    col[0] += m[0];
    col[1] += m[1];
    col[2] += m[2];
    return *this;
  }
  // Subtract a csp_matrix33 from this one
  csp_matrix33      &operator -= (const csp_matrix33 &m) {
    col[0] -= m[0];
    col[1] -= m[1];
    col[2] -= m[2];
    return *this;
  }
  // Multiply the csp_matrix33 by another csp_matrix33
  csp_matrix33      &operator *= (const csp_matrix33 &m);
  // Multiply the csp_matrix33 by a csp_scalar
  csp_matrix33      &operator *= (csp_scalar f) {
	col[0] *= f;
    col[1] *= f;
    col[2] *= f;
    return *this;
  }
  // Are these two csp_matrix33's equal?
  friend bool       operator == (const csp_matrix33 &a, const csp_matrix33 &b) {
    return((a[0] == b[0]) && (a[1] == b[1]) && (a[2] == b[2]));
  }
  // Are these two csp_matrix33's not equal?
  friend bool       operator != (const csp_matrix33 &a, const csp_matrix33 &b) {
    return((a[0] != b[0]) || (a[1] != b[1]) || (a[2] != b[2]));
  }
  // Add two csp_matrix33's
  friend csp_matrix33   operator + (const csp_matrix33 &a, const csp_matrix33 &b) {
    csp_matrix33 ret(a);
    ret += b;
    return ret;
  }
  // Subtract one csp_matrix33 from another
  friend csp_matrix33   operator - (const csp_matrix33 &a, const csp_matrix33 &b) {
    csp_matrix33 ret(a);
    ret -= b;
    return ret;
  }
  // Multiply csp_matrix33 by another csp_matrix33
  friend csp_matrix33   operator * (const csp_matrix33 &a, const csp_matrix33 &b) {
    csp_matrix33 ret(a);
    ret *= b;
    return ret;
  }
  // Multiply a csp_vector3 by this csp_matrix33
  friend csp_vector3    operator * (const csp_matrix33 &m, const csp_vector3 &v) {
    csp_vector3 ret;
    ret.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0];
    ret.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1];
    ret.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2];
    return ret;
  }
  // Multiply a csp_vector3 by this csp_matrix33
  friend csp_vector3    operator * (const csp_vector3 &v, const csp_matrix33 &m) {
    csp_vector3 ret;
    ret.x = DotProduct(m[0], v);
    ret.y = DotProduct(m[1], v);
    ret.z = DotProduct(m[2], v);
    return ret;
  }
  // Multiply csp_matrix33 by a csp_scalar
  friend csp_matrix33   operator * (const csp_matrix33 &m, csp_scalar f) {
    csp_matrix33 ret(m);
    ret *= f;
    return ret;
  }
  // Multiply csp_matrix33 by a csp_scalar
  friend csp_matrix33   operator * (csp_scalar f, const csp_matrix33 &m) {
    csp_matrix33 ret(m);
    ret *= f;
    return ret;
  }

public:
  // Methods
  // Set csp_matrix33 to the identity csp_matrix
  csp_matrix33      &identity() {
    col[0].set(1.0, 0.0, 0.0);
    col[1].set(0.0, 1.0, 0.0);
    col[2].set(0.0, 0.0, 1.0);
    return *this;
  }
  // Transpose the csp_matrix33
  csp_matrix33      &transpose();
  // Invert the csp_matrix33
  csp_matrix33      &invert();

  // Debug
  void          fprint(FILE* file, char* str) const;
};

csp_matrix33    IdentityCsp_Matrix33();
csp_matrix33    TransposeCsp_Matrix33(const csp_matrix33 &m);
csp_matrix33    InvertCsp_Matrix33(const csp_matrix33 &m);
csp_matrix33    RotateRadCsp_Matrix33(csp_scalar rad);
csp_matrix33    TranslateCsp_Matrix33(csp_scalar x, csp_scalar y);
csp_matrix33    ScaleCsp_Matrix33(csp_scalar x, csp_scalar y, csp_scalar z = 1.0);


////////////////////////////////////////////////////////////
// csp_matrix44 class
//

class csp_matrix44 
{
public:
  // Members
  csp_vector4   col[4];

public:
  // Constructors
  csp_matrix44() {};
  // Constructor with initializing value
  csp_matrix44(csp_scalar v) {
    col[0].set(v, v, v, v);
    col[1].set(v, v, v, v);
    col[2].set(v, v, v, v);
    col[3].set(v, v, v, v);
  }
  // Constructor with initializing csp_matrix44
  csp_matrix44(const csp_matrix44 &m) {
    col[0] = m[0];
    col[1] = m[1];
    col[2] = m[2];
    col[3] = m[3];
  }
  // Constructor with initializing csp_vector4's
  csp_matrix44(const csp_vector4 &v0, const csp_vector4 &v1, 
      const csp_vector4 &v2, const csp_vector4 &v3) {
    col[0] = v0;
    col[1] = v1;
    col[2] = v2;
    col[3] = v3;
  }
  // Constructor with initializing csp_matrix33
  explicit csp_matrix44(const csp_matrix33 &m) {
    col[0] = m[0];
    col[1] = m[1];
    col[2] = m[2];
    col[3].set(0.0, 0.0, 0.0, 1.0);
  }

public:
  // Operators
  // Array indexing
  csp_vector4       &operator [] (unsigned int i) {
    assert (i<4);
    return col[i];
  }
  // Array indexing
  const csp_vector4     &operator [] (unsigned int i) const {
    assert (i<4);
    return col[i];
  }
  // Assign
  csp_matrix44      &operator =  (const csp_matrix44 &m) {
    col[0] = m[0];
    col[1] = m[1];
    col[2] = m[2];
    col[3] = m[3];
    return *this;
  }
  // Assign a csp_matrix33 to the csp_matrix44
  csp_matrix44      &operator =  (const csp_matrix33 &m) {
    col[0] = m[0];
    col[1] = m[1];
    col[2] = m[2];
    col[3].set(0.0, 0.0, 0.0, 1.0);
    return *this;
  }
  // Add a csp_matrix44 to this one
  csp_matrix44      &operator += (const csp_matrix44 &m) {
    col[0] += m[0];
    col[1] += m[1];
    col[2] += m[2];
    col[3] += m[3];
    return *this;
  }
  // Subtract a csp_matrix44 from this one
  csp_matrix44      &operator -= (const csp_matrix44 &m) {
    col[0] -= m[0];
    col[1] -= m[1];
    col[2] -= m[2];
    col[3] -= m[3];
    return *this;
  }
  // Multiply the csp_matrix44 by another csp_matrix44
  csp_matrix44      &operator *= (const csp_matrix44 &m);
  // Multiply the csp_matrix44 by a csp_scalar
  csp_matrix44      &operator *= (csp_scalar f) {
    col[0] *= f;
    col[1] *= f;
    col[2] *= f;
    col[3] *= f;
    return *this;
  }
  //csp_matrix44      &operator /= (csp_scalar f) {}
  // Are these two csp_matrix44's equal?
  friend bool       operator == (const csp_matrix44 &a, const csp_matrix44 &b) {
    return((a[0] == b[0]) && (a[1] == b[1]) &&
        (a[2] == b[2]) && (a[3] == b[3]));
  }
  // Are these two csp_matrix44's not equal?
  friend bool       operator != (const csp_matrix44 &a, const csp_matrix44 &b) {
    return((a[0] != b[0]) || (a[1] != b[1]) ||
        (a[2] != b[2]) || (a[3] != b[3]));
  }
  // Add two csp_matrix44's
  friend csp_matrix44   operator + (const csp_matrix44 &a, const csp_matrix44 &b) {
    csp_matrix44 ret(a);
    ret += b;
    return ret;
  }
  // Subtract one csp_matrix44 from another
  friend csp_matrix44   operator - (const csp_matrix44 &a, const csp_matrix44 &b) {
    csp_matrix44 ret(a);
    ret -= b;
    return ret;
  }
  // Multiply csp_matrix44 by another csp_matrix44
  friend csp_matrix44   operator * (const csp_matrix44 &a, const csp_matrix44 &b) {
    csp_matrix44 ret(a);
    ret *= b;
    return ret;
  }
  // Multiply a csp_vector3 by this csp_matrix44
  friend csp_vector3    operator * (const csp_matrix44 &m, const csp_vector3 &v) {
    csp_vector4 ret(v);
    ret = m * ret;
    return csp_vector3(ret.x, ret.y, ret.z);
  }
  // Multiply a csp_vector3 by this csp_matrix44
  friend csp_vector3    operator * (const csp_vector3 &v, const csp_matrix44 &m) {
    csp_vector4 ret(v);
    ret = ret * m;
    return csp_vector3(ret.x, ret.y, ret.z);
  }
  // Multiply a csp_vector4 by this csp_matrix44
  friend csp_vector4    operator * (const csp_matrix44 &m, const csp_vector4 &v) {
    csp_vector4 ret;
    ret.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0];
    ret.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1];
    ret.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2];
    ret.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3];
    return ret;
  }
  // Multiply a csp_vector4 by this csp_matrix44
  friend csp_vector4    operator * (const csp_vector4 &v, const csp_matrix44 &m) {
    csp_vector4 ret;
    ret.x = DotProduct(m[0], v);
    ret.y = DotProduct(m[1], v);
    ret.z = DotProduct(m[2], v);
    ret.w = DotProduct(m[3], v);
    return ret;
  }
  // Multiply csp_matrix44 by a csp_scalar
  friend csp_matrix44   operator * (const csp_matrix44 &m, csp_scalar f) {
    csp_matrix44 ret(m);
    ret *= f;
    return ret;
  }
  // Set csp_matrix44 to the identity csp_matrix
  friend csp_matrix44   operator * (csp_scalar f, const csp_matrix44 &m) {
    csp_matrix44 ret(m);
    ret *= f;
    return ret;
  }

public:
  // Methods
  // Set csp_matrix44 to the identity csp_matrix
  csp_matrix44      &identity() {
    col[0].set(1.0, 0.0, 0.0, 0.0);
    col[1].set(0.0, 1.0, 0.0, 0.0);
    col[2].set(0.0, 0.0, 1.0, 0.0);
    col[3].set(0.0, 0.0, 0.0, 1.0);
    return *this;
  }
  // Transpose the csp_matrix44
  csp_matrix44      &transpose();
  // Invert the csp_matrix44
  csp_matrix44      &invert();

  // Debug
  void          fprint(FILE* file, char* str) const;
};

csp_matrix44    IdentityMatrix44();
csp_matrix44    TransposeMatrix44(const csp_matrix44 &m);
csp_matrix44    InvertMatrix44(const csp_matrix44 &m);
csp_matrix44    RotateRadCsp_Matrix44(char axis, csp_scalar rad);
csp_matrix44    RotateRadMatrix44(const csp_vector3 &axis, csp_scalar rad);
csp_matrix44    TranslateMatrix44(csp_scalar x, csp_scalar y, csp_scalar z);
csp_matrix44    ScaleMatrix44(csp_scalar x, csp_scalar y, csp_scalar z, csp_scalar w = 1.0);
csp_matrix44    LookAtMatrix44(const csp_vector3 &camPos, const csp_vector3 &camUp, 
    const csp_vector3 &target );
csp_matrix44    FrustumMatrix44(csp_scalar l, csp_scalar r, csp_scalar b, csp_scalar t, 
    csp_scalar n, csp_scalar f);
csp_matrix44    PerspectiveMatrix44(csp_scalar fovY, csp_scalar aspect, 
    csp_scalar n, csp_scalar f);
csp_matrix44    OrthoMatrix44(csp_scalar l, csp_scalar r, csp_scalar b, csp_scalar t, 
    csp_scalar n, csp_scalar f);
csp_matrix44    OrthoNormalMatrix44(const csp_vector3 &xdir, 
    const csp_vector3 &ydir, const csp_vector3 &zdir);

#endif
