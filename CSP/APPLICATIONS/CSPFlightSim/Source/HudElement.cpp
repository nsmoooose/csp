#include <algorithm>
#include <iomanip>
#include <sstream>

#include "HudElement.h"

HudElement::HudElement(std::string const & p_selementName, osg::Vec3 const & p_position):
m_position(p_position)
{
  m_selementName = p_selementName;
  std::transform(m_selementName.begin(), m_selementName.end(), m_selementName.begin(), toupper);
}

HudElement::~HudElement()
{
}

std::string HudElement::GetHudElementName() const
{
	return m_selementName;
}
