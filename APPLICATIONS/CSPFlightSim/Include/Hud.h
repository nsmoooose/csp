#ifndef HUD_H
#define HUD_H

#include <string>

// testing various osg objects
#include <osg/MatrixTransform>

#include "HudTextElement.h"
#include "TypesVector3.h"


/**
 * class Hud - Describe me!
 *
 * @author unknown
 */
class Hud : public osg::MatrixTransform
{
	HudTextElement ReadHudTextElement(std::istream & p_istream) const;
public:
	Hud(StandardVector3 const & p_direction);
	virtual ~Hud();	
	virtual void BuildHud();
	virtual void BuildHud(std::string const & p_hudFileName);
	void OnUpdate();
};


#endif