// Combat Simulator Project
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


%{
#include <csp/cspsim/Projection.h>
%}

namespace csp {

class Projection : public Referenced {
public:
    virtual csp::LLA getCenter() const = 0;
	virtual csp::Vector3 convert(csp::LLA const &pos) const = 0;
	virtual csp::LLA convert(csp::Vector3 const &pos) const = 0;
	virtual csp::Vector3 getNorth(csp::LLA const &pos) const = 0;
	virtual csp::Vector3 getNorth(csp::Vector3 const &pos) const = 0;
	virtual csp::Vector3 getUp(csp::Vector3 const &pos) const = 0;
};

class GnomonicProjection : public Projection {
public:
	double getRadius() const;
	void setRadius(double radius);

	virtual csp::LLA getCenter() const;
	void setCenter(csp::LLA const &center);

	virtual csp::Vector3 convert(csp::LLA const &pos) const;
	virtual csp::LLA convert(csp::Vector3 const &pos) const;
	virtual csp::Vector3 getNorth(csp::LLA const &pos) const;
	virtual csp::Vector3 getNorth(csp::Vector3 const &pos) const;
};

class SecantGnomonicProjection : public GnomonicProjection {
public:
    SecantGnomonicProjection();
    void set(csp::LLA const &center, double size_x, double size_y);
    
	virtual csp::Vector3 getNorth(csp::Vector3 const &pos) const;
	virtual csp::Vector3 getUp(csp::Vector3 const &pos) const;
};

} // namespace
