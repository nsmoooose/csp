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
 * @file StringResourceManager.h
 *
 **/

#ifndef __CSPSIM_WF_STRINGRESOURCEMANAGER_H__
#define __CSPSIM_WF_STRINGRESOURCEMANAGER_H__

#include <map>
#include <csp/csplib/util/Referenced.h>
#include <csp/cspwf/Export.h>
#include <csp/cspwf/Serialization.h>

namespace csp {
namespace wf {

class ResourceLocator;

typedef std::map<std::string, std::string> StringMap;

/** Hold strings for localization of texts. All texts that are used in
 * the user interface will need to be localized to a local language.
 * So a Label control can have the following text assigned: ${instant_action}.
 * When the text is going to be displayed the instant_action will be
 * replaced with a proper text from this class.
 *
 * String tables are usually placed into their own xml files and is
 * serialized into this class.
 */
class CSPWF_EXPORT StringResourceManager : public Referenced {
public:
	StringResourceManager();
	virtual ~StringResourceManager();

	virtual std::string getString(const std::string& key) const;
	virtual void merge(StringResourceManager* stringsToMerge);

	virtual std::string parseAndReplace(const std::string& original) const;

	template<class Archive>
	void serialize(Archive & ar)	{
		ar & make_nvp("Strings", m_Values);
	}

private:
	StringMap m_Values;
};

} // namespace wf
} // namespace csp

#endif // __CSPSIM_WF_STRINGRESOURCEMANAGER_H__

