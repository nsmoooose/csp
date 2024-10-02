#pragma once
// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file DataEntryDisplay.h
 *
 **/

#include <csp/cspsim/System.h>
#include <csp/cspsim/hud/Display.h>
#include <csp/csplib/data/Vector3.h>

#include <osg/ref_ptr>
namespace osg { class Group; }
namespace osgText { class Text; }

namespace csp {
namespace hud { class DisplayFont; }

class AlphaNumericDisplay;

class DataEntryDisplay: public System {
public:
	CSP_DECLARE_OBJECT(DataEntryDisplay)

	DataEntryDisplay();
	virtual ~DataEntryDisplay();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

private:
	virtual void postCreate();
	virtual double onUpdate(double dt);
	void initDisplay();

	Vector3 m_Color;
	double m_TextHeight;
	double m_LineSpacing;

	unsigned m_LineCount;
	osg::ref_ptr<osgText::Text> *m_Lines;
	int m_DirtyCount;

	DataChannel<hud::Display>::RefT b_Display;
	DataChannel<Ref<AlphaNumericDisplay> >::CRefT b_DEDReadout;
	osg::ref_ptr<hud::DisplayFont> m_StandardFont;
	osg::ref_ptr<osg::Group> m_Display;
};

} // end namespace csp
