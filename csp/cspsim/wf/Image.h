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

#ifndef __CSPSIM_WF_IMAGE_H__
#define __CSPSIM_WF_IMAGE_H__

#include <csp/cspsim/wf/Control.h>

CSP_NAMESPACE

namespace wf {

/** A Control that represents an image. You set the image to display using
 * the backgroundImage property on the style object. Do not forget to also set
 * backgroundColor as this is controling the alpha channel.
 */ 
class CSPSIM_EXPORT Image : public Control {
public:
	Image();
	virtual ~Image();

	virtual std::string getName() const;
	
	virtual void buildGeometry();	
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_IMAGE_H__

