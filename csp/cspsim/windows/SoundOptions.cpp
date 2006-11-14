// Combat Simulator Project
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file SoundOptions.cpp
 *
 **/

#include <csp/cspsim/windows/SoundOptions.h>

#include <osg/Vec4>

CSP_NAMESPACE

namespace windows {

SoundOptions::SoundOptions(wf::Theme* theme) : wf::TabPage(theme) {
	setText("Sound");
}

SoundOptions::~SoundOptions() {
}

} // namespace windows

CSP_NAMESPACE_END
