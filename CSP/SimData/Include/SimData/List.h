/* SimData: Data Infrastructure for Simulations
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
 *
 * This file is part of SimData.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/**
 * @file List.h
 * @brief A templated vector class implementing the BaseType interface
 */

#ifndef __SIMDATA_LIST_H__
#define __SIMDATA_LIST_H__

#include <string>
#include <vector>
#include <SimData/BaseType.h>
#include <SimData/Archive.h>


NAMESPACE_SIMDATA


/**
 * @brief Base class for packable lists.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class ListBase: public BaseType {
public:
	virtual ~ListBase() {}
};


/**
 * @brief A packable list class based on std::vector.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 * @ingroup BaseTypes
 */
template <class T> class List: public ListBase, public std::vector<T> {

public:

	~List() {}

	// can be used by python code to extend a list of unknown type,
	// assigning or operating on the added element as necessary.
	T& extend();

	/** Serialize to or from a data archive.
	 */
	virtual void serialize(Reader&);
	virtual void serialize(Writer&) const;

	std::string asString() const;

};


template<typename T>
T& List<T>::extend() {
	T x;
	this->push_back(x);
	return this->operator[](this->size()-1);
}

template<typename T>
void List<T>::serialize(Reader &reader) {
	reader >> static_cast<std::vector<T>&>(*this);
}

template<typename T>
void List<T>::serialize(Writer &writer) const {
	writer << static_cast<const std::vector<T>&>(*this);
}

template<typename T>
std::string List<T>::asString() const {
	std::stringstream ss;
	ss << "<simdata::List[" << size() << ">";
	return ss.str();
}


NAMESPACE_SIMDATA_END


#endif //__SIMDATA_LIST_H__

