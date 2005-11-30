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
#include <csp/cspsim/VirtualScene.h>
%}

namespace csp {

class VirtualScene {
private:
	// this is a public ctor but we declare it as private here to prevent
	// VirtualScene instances from being created from python.
	VirtualScene(int x, int y);

public:
	void addObject(csp::Ref<csp::DynamicObject> object);
	void removeObject(csp::Ref<csp::DynamicObject> object);

	void spinTheWorld(bool spin);
	void resetSpin();

	void _setLookAt(csp::Vector3 const & eyePos, csp::Vector3 const & lookPos, csp::Vector3 const & upVec);
	void getLookAt(csp::Vector3 & eyePos, csp::Vector3 & lookPos, csp::Vector3 & upVec) const;
	void setWireframeMode(bool flag);
	void setFogMode(bool flag);
	void setFogStart(float value);
	void setFogEnd(float value);

	float getViewDistance() const { return m_ViewDistance; }
	void setViewDistance(float value);

	void setCameraNode(osg::Node * pNode);

	int getTerrainPolygonsRendered();

	float getViewAngle() const { return m_ViewAngle; }
	void setViewAngle(float);

	double getSpin();
};

} // namespace

