#ifndef HUD_TEXT_ELEMENT_H
#define HUD_TEXT_ELEMENT_H

#include <fstream>

#include <osg/Vec4>
#include <osgText/Text>

#include "HudElement.h"

struct DrawableAppCallback;

// hud text element

/**
 * class HudTextElement - Describe me!
 *
 * @author unknown
 */
class HudTextElement : public HudElement, public osgText::Text
{
	typedef void (HudTextElement::*TypeSetValue)();
	typedef std::map< std::string, TypeSetValue > FuncMap;
	
	std::string m_sstaticString;
	char m_cstaticStringPosition;
	std::string m_fontPath;
	unsigned short m_fontSize;
	float m_fvalue;
	unsigned short m_usiformat;
	
	bool static m_initializedSetValue;      // initialization flag for SetValue functions map 
	FuncMap static m_SetValueSet;           // Set of setter functions
	
	virtual void SetValueHEAD();
	void SetValueAOA();
	void SetValueG();
	void SetValueKT();
	void SetValueMACH();
	void SetValueALTFT();
    void SetValueALTR();
    void SetValueALTS();
    void SetValueNWP();
    void SetValueWPH();
    void SetValueWPD();
    void SetValuePALT1();
    void SetValuePALT2();
    void SetValueRPM();
	void SetValue();
	void InitFuncMap();
    virtual void OnUpdate();
public:
	HudTextElement(){;};
	HudTextElement(std::string const & p_selementName, osg::Vec3 const & p_position, 
		std::string const & p_sstaticString, char const p_staticStringPosition = 'p', 
		std::string const & p_fontName = "ltypeb", unsigned short const p_fontSize = 16, 
		unsigned short const p_usiformat = 0);
	virtual ~HudTextElement();
	void OnUpdate(float p_fvalue);
	void OnUpdate(float p_fvalue, osg::Vec3 const & p_position);
	void OnUpdate(float p_fvalue, osg::Vec3 const & p_position, float p_fangle);

	void SetValue(float p_fvalue);
	void SetValue(std::string const & p_svalue);
	float GetValue() const;
	friend struct DrawableAppCallback;
};

#endif

