// Combat Simulator Project
// Copyright (C) 2004-2005 The Combat Simulator Project
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
 * @file IsoContour.h
 *
 **/

#ifndef __THEATER_ISOCONTOUR_H__
#define __THEATER_ISOCONTOUR_H__


#include <SimData/Vector3.h>
#include <SimData/Object.h>
#include <SimData/InterfaceRegistry.h>
#include <SimCore/Util/Log.h>


class IsoContour: public simdata::Object {
protected:
	virtual float f(float x, float y) const = 0;
public:
	virtual bool in(float x, float y) const {
		return f(x,y) < 1.0f;
	}
	virtual simdata::Vector3 getPoint(float x,float y) const = 0;
	virtual float getArea() const = 0;
	virtual ~IsoContour() {}
};


class Circle: public IsoContour {
	float m_Radius;
protected:
	virtual float f(float x, float y) const {
		return x*x+y*y;
	}
public:
	SIMDATA_DECLARE_OBJECT(Circle)

	Circle(float radius = 20.0f):
	  m_Radius(radius) {
	}
	virtual simdata::Vector3 getPoint(float x, float y) const {
		return simdata::Vector3(m_Radius * x, m_Radius * y, 0.0f);
	}
	virtual float getArea() const {
		return static_cast<float>(simdata::PI * m_Radius * m_Radius);
	}
	virtual ~Circle() {}
};


class RectangularCurve: public IsoContour {
	float m_Width, m_Height;
protected:
	virtual float f(float /*x*/, float /*y*/) const {
		return 0.0f;
	}
public:
	SIMDATA_DECLARE_OBJECT(RectangularCurve)

	RectangularCurve(float width = 20.0f, float height = 50.0f) {
		m_Width = (width != 0.0f) ? fabs(width) : 20.0f;
		m_Height = (height != 0.0f) ? fabs(height) : 50.0f;
		if (width * height == 0.0f) {
			CSP_LOG(APP, WARNING, "Rectangle: [" << width << "," << height << "corrected to 20x50]");
		}
	}
	virtual simdata::Vector3 getPoint(float x, float y) const {
		return simdata::Vector3(m_Width * x, m_Height * y, 0.0f);
	}
	virtual float getArea() const {
		return m_Width * m_Height;
	}
	virtual ~RectangularCurve(){}
};


#endif // __THEATER_ISOCONTOUR_H__

