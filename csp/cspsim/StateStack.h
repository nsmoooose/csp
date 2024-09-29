// Combat Simulator Project
// Copyright (C) 2006 The Combat Simulator Project
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

// Portions of this code taken from OpenSceneGraph, Copyright (C)
// 1998-2005 Robert Osfield.  OpenSceneGraph is distributed under the
// OpenSceneGraph Public License, version 0.0.  All code in this file,
// including portions adapted from OpenSceneGraph, is licensed under
// the GNU General Public License.

#ifndef __CSPSIM_STATESTACK_H__
#define __CSPSIM_STATESTACK_H__

#include <csp/csplib/util/Namespace.h>

#include <osg/StateAttribute>
#include <osg/StateSet>

#include <cassert>
#include <map>
#include <vector>

#ifdef _WIN32
#include <assert.h>
#endif

namespace csp {

/**
 * This class duplicates some of the code in osg::State to track cumulative
 * state in a scene graph.  This is necessary because osg::State does not
 * expose enough of the cumulative state (e.g., captureCurrentState excludes
 * texture modes and attributes).  Although osg::State could be subclassed to
 * provide access to these values (the necessary internal data structures are
 * protected), the logic has been copied to avoid depending on internal aspects
 * of the OSG api.
 */

class StateStack {
public:
	typedef std::pair<osg::StateAttribute const *, osg::StateAttribute::OverrideValue> AttributePair;
	~StateStack() { assert(m_StateSetStack.empty()); }

	void push(osg::StateSet const *ss) {
		m_StateSetStack.push_back(ss);
		if (ss) {
			pushModeList(m_ModeMap, ss->getModeList());
			pushAttributeList(m_AttributeMap, ss->getAttributeList());
			const osg::StateSet::TextureModeList &tml = ss->getTextureModeList();
			for (unsigned unit = 0; unit < tml.size(); ++unit) {
				pushModeList(getOrCreateTextureModeMap(unit), tml[unit]);
			}
			const osg::StateSet::TextureAttributeList &tal = ss->getTextureAttributeList();
			for (unsigned unit = 0; unit < tal.size(); ++unit) {
				pushAttributeList(getOrCreateTextureAttributeMap(unit), tal[unit]);
			}
		}
	}

	void pop() {
		assert(!m_StateSetStack.empty());
		if (m_StateSetStack.empty()) return;
		osg::StateSet const *ss = m_StateSetStack.back();
		if (ss) {
			popModeList(m_ModeMap, ss->getModeList());
			popAttributeList(m_AttributeMap, ss->getAttributeList());
			const osg::StateSet::TextureModeList &tml = ss->getTextureModeList();
			for (unsigned unit = 0; unit < tml.size(); ++unit) {
				popModeList(getOrCreateTextureModeMap(unit), tml[unit]);
			}
			const osg::StateSet::TextureAttributeList &tal = ss->getTextureAttributeList();
			for (unsigned unit = 0; unit < tal.size(); ++unit) {
				popAttributeList(getOrCreateTextureAttributeMap(unit), tal[unit]);
			}
		}
		m_StateSetStack.pop_back();
	}

	osg::StateAttribute::GLModeValue getMode(osg::StateAttribute::GLMode mode) const {
		ModeMap::const_iterator iter = m_ModeMap.find(mode);
		if (iter == m_ModeMap.end() || iter->second.empty()) return static_cast<osg::StateAttribute::GLModeValue>(osg::StateAttribute::OFF);
		return iter->second.back();
	}

	AttributePair getAttribute(osg::StateAttribute::Type type, int member=0) const {
		AttributeMap::const_iterator iter = m_AttributeMap.find(TypeMemberPair(type, member));
		if (iter == m_AttributeMap.end() || iter->second.empty()) return AttributePair(0, osg::StateAttribute::OFF);
		return iter->second.back();
	}

	osg::StateAttribute::GLModeValue getTextureMode(unsigned unit, osg::StateAttribute::GLMode mode) const {
		if (unit < m_TextureModeMapList.size()) {
			ModeMap const &mm = m_TextureModeMapList[unit];
			ModeMap::const_iterator iter = mm.find(mode);
			if (iter != mm.end() && !iter->second.empty()) {
				return iter->second.back();
			}
		}
		return osg::StateAttribute::OFF;
	}

	AttributePair getTextureAttribute(unsigned unit, osg::StateAttribute::Type type, int member=0) const {
		if (unit < m_TextureAttributeMapList.size()) {
			AttributeMap const &am = m_TextureAttributeMapList[unit];
			AttributeMap::const_iterator iter = am.find(TypeMemberPair(type, member));
			if (iter != am.end() && !iter->second.empty()) {
				return iter->second.back();
			}
		}
		return AttributePair(0, osg::StateAttribute::OFF);
	}

private:
	typedef osg::StateAttribute::TypeMemberPair TypeMemberPair;
	typedef std::vector<osg::StateAttribute::GLModeValue> ModeStack;
	typedef std::map<osg::StateAttribute::GLMode, ModeStack> ModeMap;
	typedef std::vector<ModeMap> TextureModeMapList;
	typedef std::vector<AttributePair> AttributeStack;
	typedef std::map<TypeMemberPair, AttributeStack> AttributeMap;
	typedef std::vector<AttributeMap> TextureAttributeMapList;
	typedef std::vector<osg::StateSet const *> StateSetStack;

	ModeMap &getOrCreateTextureModeMap(unsigned unit) {
		if (m_TextureModeMapList.size() <= unit) {
			m_TextureModeMapList.resize(unit + 1);
		}
		return m_TextureModeMapList[unit];
	}

	AttributeMap &getOrCreateTextureAttributeMap(unsigned unit) {
		if (m_TextureAttributeMapList.size() <= unit) {
			m_TextureAttributeMapList.resize(unit + 1);
		}
		return m_TextureAttributeMapList[unit];
	}

	void pushModeList(ModeMap &mode_map, osg::StateSet::ModeList const &mode_list) {
		for (osg::StateSet::ModeList::const_iterator iter = mode_list.begin(); iter != mode_list.end(); ++iter) {
			ModeStack &ms = mode_map[iter->first];
			if (!ms.empty() && (ms.back() & osg::StateAttribute::OVERRIDE) && (!(iter->second & osg::StateAttribute::PROTECTED))) {
				ms.push_back(ms.back());
			} else {
				ms.push_back(iter->second);
			}
		}
	}

	void popModeList(ModeMap &mode_map, osg::StateSet::ModeList const &mode_list) {
		for (osg::StateSet::ModeList::const_iterator iter = mode_list.begin(); iter != mode_list.end(); ++iter) {
			ModeStack &ms = mode_map[iter->first];
			if (!ms.empty()) ms.pop_back();
		}
	}

	void pushAttributeList(AttributeMap &attr_map, osg::StateSet::AttributeList const &attr_list) {
		for (osg::StateSet::AttributeList::const_iterator iter = attr_list.begin(); iter != attr_list.end(); ++iter) {
			AttributeStack &as = attr_map[iter->first];
			if (!as.empty() && (as.back().second & osg::StateAttribute::OVERRIDE) && (!(iter->second.second & osg::StateAttribute::PROTECTED))) {
				as.push_back(as.back());
			} else {
				as.push_back(AttributePair(iter->second.first.get(), iter->second.second));
			}
		}
	}

	void popAttributeList(AttributeMap &attr_map, osg::StateSet::AttributeList const &attr_list) {
		for (osg::StateSet::AttributeList::const_iterator iter = attr_list.begin(); iter != attr_list.end(); ++iter) {
			AttributeStack &as = attr_map[iter->first];
			if (!as.empty()) as.pop_back();
		}
	}

	StateSetStack m_StateSetStack;
	ModeMap m_ModeMap;
	TextureModeMapList m_TextureModeMapList;
	AttributeMap m_AttributeMap;
	TextureAttributeMapList m_TextureAttributeMapList;
};

} // namespace csp

#endif  // __CSPSIM_STATESTACK_H__

