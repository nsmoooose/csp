#pragma once
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
 * @file Image.h
 *
 **/

#include <csp/cspwf/Control.h>

namespace csp {
namespace wf {

/** A Control that represents an image. You set the image to display using
 * the backgroundImage property on the style object. Do not forget to also set
 * backgroundColor as this is controling the alpha channel.
 */ 
class CSPWF_EXPORT Image : public Control {
public:
	Image();
	virtual ~Image();

	virtual void performLayout();	
};

} // namespace wf
} // namespace csp
