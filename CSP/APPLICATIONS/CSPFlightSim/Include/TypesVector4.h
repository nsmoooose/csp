// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file TypesVector4.h
 *
 **/

#ifndef __STANDARDVECTOR4_H__
#define __STANDARDVECTOR4_H__

// Initial CSP version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.

/** \Class StandardVector4
 * This class represents a 4 element vector. It uses overloaded
 * operators to preform basic operations.
 */



class StandardVector4
{
 public:
  double x;
  double y;
  double z;
  double w;

    /** StandardVector4()
     * Default constructor
     */ 
    StandardVector4();

    /** StandardVector4(double x, double y, double z, double w)
     * Constructor from doubles
     */
    StandardVector4(double x, double y, double z, double w);

    /** StandardVector4(StandardVector4 &v) 
     * Copy constructor
     */
    StandardVector4(const StandardVector4 &v);

    /** Array Indexing
     */
    double & operator[] (int i);
    const double & operator[] (int i) const;
    operator double * ();

    /** Arithmatic assignment operators
     */
    StandardVector4 & operator=(const StandardVector4 & v);
    StandardVector4 & operator+=(const StandardVector4 & v);
    StandardVector4 & operator-=(const StandardVector4 & v);
    StandardVector4 & operator*=(double f);
    StandardVector4 & operator/=(double f);

    /** Arithmatic operator
     */
    //    StandardVector4 operator+(const StandardVector4 &v) const;
    //    StandardVector4 operator-(const StandardVector4 &v) const;
    //    StandardVector4 operator*(const double f) const;
    //    StandardVector4 operator/(const double f) const;

    /** Equality operators
     */
    bool operator==(const StandardVector4 &a);
    bool operator!=(const StandardVector4 &a);

    /** Negation operator
     */
    StandardVector4 operator - () const;
    
    /** friend arithmatic operators
     */
    friend StandardVector4 operator+(const StandardVector4 &a, const StandardVector4 &b);
    StandardVector4 operator-(const StandardVector4 &b) const;
    friend StandardVector4 operator*(const StandardVector4 &a , double f);
    friend StandardVector4 operator*(double f, const StandardVector4 &a);
    friend StandardVector4 operator/(const StandardVector4 &a, double f);

    /** Additional methods
     */
    void Set(double _x, double _y, double _z, double _w);
    double Length() const;
    double LengthSquared() const;
    bool IsNull();
    StandardVector4 & Normalize();

    void Print(FILE * stream);

};

#include "TypesVector4.inl"


#endif // __TYPESVECTOR4_H__

