#ifndef HUD_GEOMETRIC_ELEMENT_H
#define HUD_GEOMETRIC_ELEMENT_H

#include <osg/MatrixTransform>

#include "HudElement.h"

// hud geometric element


/**
 * class HudGeometricElement - Describe me!
 *
 * @author unknown
 */
class HudGeometricElement : public HudElement, public osg::MatrixTransform
{
	float m_fheadingAngle;
	float m_fpitchAngle;
public:
	 HudGeometricElement(){;};
	 HudGeometricElement(std::string const & p_selementName, osg::Vec3 const & p_position, 
		                 float const p_fheadingAngle = 0.0, float const p_fpitchAngle = 0.0);
	 virtual ~HudGeometricElement();
	 void MakeHsi();
	 void MakeFpm(float const p_radius = 10.0, float const p_segLengthH = 10.0, float const p_segLengthV = 5.0);
	 virtual void OnUpdate();
     void OnUpdate(osg::Vec3 const & p_position, float p_fangle = 0.0);
	 void OnUpdateHsi();
	 void OnUpdateFpm();
};



#endif