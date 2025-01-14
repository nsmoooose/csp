#pragma once
// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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
 * @file ObjectModel.h
 *
 **/

#include <csp/cspsim/Export.h>

#include <csp/csplib/data/Enum.h>
#include <csp/csplib/data/External.h>
#include <csp/csplib/data/Link.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Vector3.h>

#include <osg/ref_ptr>
#include <vector>

namespace osg {
	class Node;
	class Switch;
	class MatrixTransform;
}

namespace osgText {
	class Text;
}

namespace csp {

class Animation;
class Bus;
class Station;


/**
 * class ObjectModel - Static representation of 3D models in the simulation.
 *
 * One ObjectModel instance is created for each type of model, and shared by
 * many SceneModel instances.
 *
 * TODO: add LOD support here.
 *
 */
class CSPSIM_EXPORT ObjectModel: public Object {
	static const Enumeration EffectItems;

public:
	typedef std::vector<Vector3> PointList;

	CSP_DECLARE_STATIC_OBJECT(ObjectModel)

	/** This class is responsible for loading all models within the
	 * simulator. By using this method call you can add all paths
	 * used to find images and model files. */
	static void setDataFilePathList(std::string search_path);

	ObjectModel();
	virtual ~ObjectModel();

	std::string getModelPath() const;
	void setModelPath(const External& path);

	bool getSmooth() const;
	void setSmooth(bool smooth);

	/** Loads the;3P scenegraph model from disc. */
	virtual void loadModel();


	osg::ref_ptr<osg::Node> getModel();
	osg::ref_ptr<osg::Node> getDebugMarkers();
	osg::ref_ptr<osg::Node> getGroundShadow();

	const Vector3 &getAxis0() const { return m_Axis0; }
	const Vector3 &getAxis1() const { return m_Axis1; }
	const Vector3 &getViewPoint() const { return m_ViewPoint; }
	const Vector3 &getHudPlacement() const { return m_HudPlacement; }
	double getHudWidth() const { return m_HudWidth; }
	double getHudHeight() const { return m_HudHeight; }

	unsigned numAnimations() const { return m_Animations.size(); }
	const Animation &animation(unsigned i) const {
		assert(i < numAnimations());
		return *(m_Animations[i]);
	}

	double getBoundingSphereRadius() const { return m_BoundingSphereRadius; }
	PointList const &getContacts() const { return m_Contacts; }
	std::string const &getLabel() const { return m_Label; }

	bool getElevationCorrection() const { return m_ElevationCorrection; }
	bool isFlat() const { return m_PolygonOffset != 0.0; }

	void showDebugMarkers(bool on);
	bool getDebugMarkersVisible() const;

	unsigned numStations() const { return m_Stations.size(); }
	const Station *station(unsigned i) const {
		assert(i < numStations());
		return m_Stations[i].get();
	}

protected:
	std::string m_Label;
	External m_ModelPath;
	External m_GroundShadowPath;
	Vector3 m_Axis0, m_Axis1;
	Vector3 m_Offset;
	Vector3 m_ViewPoint;
	Vector3 m_HudPlacement;
	double m_HudWidth;
	double m_HudHeight;
	double m_Scale;

	bool m_Smooth;
	bool m_Filter;
	float m_FilterValue;
	Enum<EffectItems> m_Effect;

	bool m_ElevationCorrection;
	float m_PolygonOffset;
	int m_CullFace;
	bool m_Lighting;

	PointList m_Contacts;
	PointList m_DebugPoints;

	Link<Animation>::vector m_Animations;
	Link<Station>::vector m_Stations;

	virtual void postCreate();
	void processModel();
	void addDebugMarkers();
	void generateStationMasks(std::map<std::string, unsigned> const &interior_map) const;

	double m_BoundingSphereRadius;

	enum { DEBUG_MARKERS };

private:

	osg::ref_ptr<osg::MatrixTransform> m_Transform;
	osg::ref_ptr<osg::Node> m_Model;
	osg::ref_ptr<osg::Node> m_GroundShadow;
	osg::ref_ptr<osg::Switch> m_DebugMarkers;
	osg::ref_ptr<osg::Switch> m_ContactMarkers;
};

} // namespace csp
