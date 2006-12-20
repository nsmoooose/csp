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
 * @file MessageBox.h
 *
 **/

#ifndef __CSPSIM_WF_MESSAGEBOX_H__
#define __CSPSIM_WF_MESSAGEBOX_H__

#include <csp/cspsim/wf/Window.h>

CSP_NAMESPACE

namespace wf {

class Label;

class MessageBox : public Window {
public:
	MessageBox(std::string caption, std::string message);
	virtual ~MessageBox();

	virtual void buildGeometry();
	
	virtual std::string getMessage() const;
	virtual void setMessage(std::string message);
	
	static Ref<MessageBox> Show(std::string caption, std::string message);
	
private:
	Ref<Label> m_Label;	
	virtual void onOk();
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_MESSAGEBOX_H__

