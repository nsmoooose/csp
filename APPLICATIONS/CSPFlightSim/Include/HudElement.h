#ifndef HUD_ELEMENT_H
#define HUD_ELEMENT_H

#include <string>

#include <osg/Vec3>


// every "object" in the hud should inherite from this abstract class

/**
 * class HudElement - Describe me!
 *
 * @author unknown
 */
class HudElement
{
protected:
    std::string m_selementName;
	osg::Vec3 m_position;
public:
	 HudElement(){;};
	 HudElement(std::string const & p_selementName, osg::Vec3 const & p_position);
	 virtual ~HudElement();
	 virtual void OnUpdate() = 0;
	 std::string GetHudElementName() const;
	 //virtual void drawImmediateMode(osg::State& state) {};
	 //virtual const bool computeBound() const { return false;}
};

#endif

