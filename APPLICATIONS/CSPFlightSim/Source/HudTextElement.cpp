#include <iomanip>
#include <sstream>



#include "AirplaneObject.h"
#include "BaseObject.h"
#include "HudTextElement.h"
#include "VirtualBattlefield.h"

// should be a vector of colors; iterate on this vector to change color
osg::Vec4 fontColor(.9f,.9f,.6f,1.0f);

extern BaseObject * g_pPlayerObject;
extern VirtualBattlefield * g_pBattlefield;

HudTextElement::FuncMap HudTextElement::m_SetValueSet;

bool HudTextElement::m_initializedSetValue = false;


/**
 * struct DrawableAppCallback
 *
 * @author unknown
 */
struct DrawableAppCallback : public osg::Drawable::AppCallback
{
	virtual void app(osg::NodeVisitor*, osg::Drawable* drawable) 
	{
		HudTextElement * aHudTextElement = dynamic_cast<HudTextElement*>(drawable);
		aHudTextElement->OnUpdate();
	}
};

void HudTextElement::InitFuncMap()
{
	m_SetValueSet["HEAD"] = &HudTextElement::SetValueHEAD;
	m_SetValueSet["KT"] = &HudTextElement::SetValueKT;
	m_SetValueSet["MACH"] = &HudTextElement::SetValueMACH;
	m_SetValueSet["AOA"] = &HudTextElement::SetValueAOA;
	m_SetValueSet["G"] = &HudTextElement::SetValueG;
	m_SetValueSet["ALTFT"] = &HudTextElement::SetValueALTFT;
    m_SetValueSet["ALTR"] = &HudTextElement::SetValueALTR;
    m_SetValueSet["ALTS"] = &HudTextElement::SetValueALTS;
    m_SetValueSet["NWP"] = &HudTextElement::SetValueNWP;
    m_SetValueSet["WPH"] = &HudTextElement::SetValueWPH;
    m_SetValueSet["WPD"] = &HudTextElement::SetValueWPD;
    m_SetValueSet["PALT1"] = &HudTextElement::SetValuePALT1;
    m_SetValueSet["PALT2"] = &HudTextElement::SetValuePALT2;
    m_SetValueSet["RPM"] = &HudTextElement::SetValueRPM;

	//setAppCallback(pdrawableAppCallback);
}

HudTextElement::HudTextElement(std::string const & p_selementName, osg::Vec3 const & p_position, 
					   std::string const & p_sstaticString, char const p_staticStringPosition, std::string const & p_fontName, 
					   unsigned short const p_fontSize, unsigned short const p_usiformat):
HudElement(p_selementName, p_position),
m_sstaticString(p_sstaticString), 
m_cstaticStringPosition(p_staticStringPosition),
m_fontPath("fonts\\" + p_fontName + ".ttf"), m_fontSize(p_fontSize),
m_usiformat(p_usiformat)
{
	if (!m_initializedSetValue)
	{
		m_initializedSetValue = true;
		InitFuncMap();
	};

    //osgText::PolygonFont* font = osgNew  osgText::PolygonFont(m_fontPath, p_fontSize, 3);		                                  
	osgText::BitmapFont* font = osgNew osgText::BitmapFont(m_fontPath, p_fontSize);

    setFont(font);
    setDrawMode( osgText::Text::TEXT );
    setAlignment(osgText::Text::LEFT_BOTTOM);
	//setAxisAlignment(XZ_PLANE);
	setColor(fontColor);	
	setPosition(p_position);
	SetValue(0.0);
	//m_osgtext->
	//setAppCallback(osgNew DrawableAppCallback());
}

HudTextElement::~HudTextElement()
{
}

void HudTextElement::SetValue()
// default SetValue fonction
{
}

void HudTextElement::SetValue(float p_fvalue)
{
	m_fvalue = p_fvalue;
	std::ostringstream ostream;
	switch ( m_cstaticStringPosition )
	{
	case 'p': 
		//ostream << m_sstaticString << " " << std::setprecision(m_usiformat) << std::fixed << m_fvalue;
		ostream << m_sstaticString << " " << std::setprecision(m_usiformat) << std::setiosflags(ios::fixed) << m_fvalue;
	 break;
	case 's' : 
		//ostream  << std::setprecision(m_usiformat) << std::fixed << m_fvalue << " " << m_sstaticString;
		ostream  << std::setprecision(m_usiformat) << std::setiosflags(ios::fixed) << m_fvalue << " " << m_sstaticString;
		break;
	case 'u' : 
		ostream  << m_sstaticString;
		break;
	}
	std::string a = ostream.str();
	setText(a);
}

void HudTextElement::SetValue(std::string const & p_svalue)
{
	switch ( m_cstaticStringPosition )
	{
	case 'p': 
		setText(m_sstaticString + " " + p_svalue);
	 break;
	case 's': 
		setText(p_svalue + " " + m_sstaticString);
		break;
	case 'u':
		setText(m_sstaticString);
		break;
	}
}

//void HudTextElement::ChangeColor()
//{
	// iterate on colors
    // 
//}

float HudTextElement::GetValue() const
{
	return m_fvalue;
}

void HudTextElement::OnUpdate()
{
	FuncMap::iterator pos = m_SetValueSet.find(m_selementName);
	if ( pos == m_SetValueSet.end() )
		SetValue();
    else 
    {
		TypeSetValue SetValue = pos->second;
		(this->*SetValue)();
	}
}

unsigned short DegreesToHudHeading( int p_angle)
{
	return (static_cast<short>(p_angle / 10.0) + 36) % 36;
}

void HudTextElement::SetValueHEAD()
{
	int heading = (static_cast<int>(g_pPlayerObject->getHeading()) + 360) % 360;
	short head1 = DegreesToHudHeading( heading );
	short head0 = head1 - 1, head2 = head1 + 1;
	std::ostringstream osvalue;
	
	if ( head1 == 0 ) 
		head0 = 35;
	else 
		if ( head1 == 35 )
			head2 = 0;
    osvalue << std::setw(2) << std::setfill('0') << head0  << std::setw(2) 
		    << std::setfill('0') << head1  << std::setw(2) << std::setfill('0') << head2;
	std::string svalue = osvalue.str();
	SetValue(svalue);
}

void HudTextElement::SetValueAOA()
{
  AirplaneObject * pplayerPlane = dynamic_cast<AirplaneObject*>(g_pPlayerObject);
  SetValue ( RadiansToDegrees(pplayerPlane->getAngleOfAttack()) );
}

void HudTextElement::SetValueG()
{
  SetValue ( g_pPlayerObject->getGForce() );
}

void HudTextElement::SetValueKT()
{
  SetValue ( MetersPerSecondToKnots(g_pPlayerObject->getSpeed()) );
}

void HudTextElement::SetValueMACH()
{
  SetValue ( MetersPerSecondToMachs(g_pPlayerObject->getSpeed()) );
}

void HudTextElement::SetValueALTFT()
{// altitue given by a sensor
  SetValue ( MetersToFeets(g_pPlayerObject->getGlobalPosition().z) );
}

void HudTextElement::SetValueALTR()
{ // altitude given by a radar system
  StandardVector3 pos = g_pPlayerObject->getGlobalPosition();
  double elev = g_pBattlefield->getElevation(pos.x,pos.y);
  SetValue ( MetersToFeets(pos.z - elev) );
}

void HudTextElement::SetValueALTS()
{ 
  //SetValue ( 6000.0 );
}

void HudTextElement::SetValueNWP()
{
  SetValue ( 10 );
}

void HudTextElement::SetValueWPH()
{
  //SetValue ( 6000 );
}

void HudTextElement::SetValueWPD()
{
  //SetValue ( 15 );
}

void HudTextElement::SetValuePALT1()
{
  SetValue ( 6000 );
}

void HudTextElement::SetValuePALT2()
{
  //SetValue ( 0 );
}

void HudTextElement::SetValueRPM()
{
  AirplaneObject * pplayerPlane = dynamic_cast<AirplaneObject*>(g_pPlayerObject);
  SetValue ( 50 * pplayerPlane->getThrottle() );
}
