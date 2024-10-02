#pragma once

#include <osg/Transform>

namespace csp {

class Sky;

/** This class is responsible for setting up the sky so we are able to look at it. 
 *  It is a group with all stars, sky dome, sun and the moon. 
 *  
 *  It also contains a transformation to make the sky follow the camera. Independent
 *  on where the camera is positioned.
 */
class CSPSIM_EXPORT SkyGroup : public osg::Transform {
public:
	SkyGroup();

	virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const;
	virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const;

	Sky* sky();

private:
	osg::ref_ptr<Sky> m_sky;
};

} // namespace csp
