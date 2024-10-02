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
 * @file ResourceBundle.h
 *
 * @brief Static object for indexing external resources (e.g. for sound samples).
 *
 **/

// TODO if no other resources fit here, rename to SoundBundle and move to csplib/sound?

#include <csp/csplib/data/Link.h>
#include <csp/csplib/data/Object.h>

#include <map>
#include <string>

namespace csp {

class SoundSample;

/** A static Object class used to specify and access a group of external resources,
 *  such as sound samples, that may be shared by multiple Systems (or other classes).
 */
class ResourceBundle: public Object {
public:
	CSP_DECLARE_STATIC_OBJECT(ResourceBundle)
	ResourceBundle();
	virtual ~ResourceBundle();
	SoundSample const *getSoundSample(std::string const &name) const;
protected:
	virtual void postCreate();
private:
	typedef std::map<std::string, Ref<SoundSample> > SoundSampleMap;
	SoundSampleMap m_SoundSampleMap;
	Link<SoundSample>::vector m_SoundSamples;
};

} // namespace csp
