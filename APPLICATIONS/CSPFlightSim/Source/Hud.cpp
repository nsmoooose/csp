#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <utility>
 
#include <osg/Depth>
#include <osg/Geode>

#include "Hud.h"
#include "HudGeometricElement.h"
#include "HudTextElement.h"

extern int g_ScreenWidth; 
extern int g_ScreenHeight;

Hud::Hud(StandardVector3 const & p_direction)
{
  double const scale = 1000.0;
  StandardVector3 t1  = scale * p_direction;
  osg::Vec3 t2 = osg::Vec3(t1.x, t1.y, t1.z);
  setMatrix( osg::Matrix::translate(t2) );
}

Hud::~Hud()
{

}

void Hud::BuildHud()
{ // testing linear code here 

}

HudTextElement Hud::ReadHudTextElement(std::istream & p_istream) const
{
 std::string selementName;
 std::string sstaticString;
 // position of static string: must be prefix (p) or sufix (s)
 char staticStringPosition;
 float x,y,z;

 p_istream  >> selementName >> x >> y >> z >> sstaticString >> staticStringPosition;
 if (sstaticString == "#") 
	 sstaticString = "";

 osg::Vec3 position( x * g_ScreenWidth , y * g_ScreenHeight, z );

 std::string sfontName;
 unsigned short fontSize;
 float fvalue;	    
 unsigned short usiformat;

 p_istream >> sfontName >> fontSize >> fvalue >> usiformat;
 if (sfontName == "#") 
	 sfontName = "arial";

 HudTextElement a = HudTextElement(selementName, position, sstaticString, staticStringPosition, sfontName, fontSize, 
	                                usiformat);
 a.SetValue(fvalue);
 return a;
}

void Hud::BuildHud(std::string const & p_hudFileName)
{
	const unsigned short lineSize = 128;

	// set hud name to this class instance
    setName(p_hudFileName);

	{
	// open the hud file
	std::ifstream iFStream(p_hudFileName.c_str());

	// first read the header of the file
	char buf[lineSize];
	// skip file name
	iFStream.getline(buf, lineSize);

	// read the "static" text part of the hud in a unique geode

    osg::MatrixTransform * staticHud = osgNew osg::MatrixTransform;
	staticHud->preMult(osg::Matrix::rotate(osg::inDegrees(90.0), 1.0, 0.0,0.0));
	
	// static part is stored in a geode which contains all osgtext drawables 
	osg::Geode * aGeode = osgNew osg::Geode;
	aGeode->setName("Static " + p_hudFileName);

    while ( iFStream.getline(buf, lineSize) ) 
	{   
		// skip comment in hud file description
		if ( buf[0] == '#' )
		{
			//CSP_LOG( CSP_APP , CSP_INFO,  "Comment line " << buf << " in " << p_hudFileName); 
			std::cout << buf << " in " << p_hudFileName << "\n";
		}
		else
		{
		HudTextElement * phudTextElement = osgNew HudTextElement;
		std::istringstream isStream(buf);
		// read every hud element
		*phudTextElement = ReadHudTextElement(isStream);	
		aGeode->addDrawable(phudTextElement);
		}
	}
	// add this static part to the hud (master transform)
    staticHud->addChild(aGeode);
    addChild(staticHud);
	iFStream.close();  
    }

	{
	// next, add hsi to the hud
	HudGeometricElement * phsi = osgNew HudGeometricElement(std::string("HSI"), osg::Vec3(0.0,0.0,0.0));
    // add hsi to hud (master transform)
	addChild(phsi);

	HudGeometricElement * pfpm = osgNew HudGeometricElement(std::string("FPM"), osg::Vec3(0.0,0.0,0.0));
	addChild(pfpm);
    }
	
    
	osg::StateSet * state = osgNew osg::StateSet;
	osg::Depth* depth = osgNew osg::Depth;
    depth->setRange(0.0,0.0);
    
    state->setAttribute(depth);
	state->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
	state->setMode(GL_LIGHTING,osg::StateAttribute::OFF); 
	setStateSet(state);
}


void Hud::OnUpdate()
{ // update all drawables in this node
	
	//std::transform(_children.begin(), _children.end(), _children.begin(), ( osg::Geode::getDrawable(0))->OnUpdate() );
	//for (ChildList::iterator NodeIt = _children.begin(); NodeIt != _children.end(); ++NodeIt)
	
	unsigned short i;
	
	// update static part
	osg::MatrixTransform * aTransform = dynamic_cast<osg::MatrixTransform *>(getChild(0));
    osg::Geode * aGeode = dynamic_cast<osg::Geode *>(aTransform->getChild(0));
	for (i = 0; i < aGeode->getNumDrawables(); ++i)
	{
		HudElement * aHudElement = dynamic_cast<HudElement *>(aGeode->getDrawable(i));	
		aHudElement->OnUpdate();
		//osgText::Text * aText = aGeode->getDrawable(i);
		//aText->app();
	}

	// update hsi
    for (i = 1; i < 3; ++i)
	{
		HudElement * aHudElement = dynamic_cast<HudElement *>(getChild(i));
		aHudElement->OnUpdate();
    }
}   




































//struct drawer 
//{ 
//	void operator() (const std::pair<std::string,HudElement> & t) 
//	{ 
//		t.second.Draw(); 
//	}
//};  

//void Hud::Draw() const
//{
//	std::map<std::string,HudElement>::const_iterator iEnd = m_hud.end();
//	std::for_each(m_hud.begin(), iEnd, drawer());
//}
