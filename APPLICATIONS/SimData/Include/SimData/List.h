/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimDataCSP.
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
 */


#ifndef __LIST_H__
#define __LIST_H__

#include <string>
#include <vector>
#include <SimData/BaseType.h>
#include <SimData/Pack.h>


NAMESPACE_SIMDATA


/**
 * Base class for packable lists.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class ListBase: public BaseType {
public:
	virtual ~ListBase() {}
};


/**
 * A packable list class based on std::vector.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
template <class T> class List: public ListBase, public std::vector<T> {

public:

	// can be used by python code to extend a list of unknown type, 
	// assigning or operating on the added element as necessary.
	/*
	T& extend();
	virtual void pack(Packer& p) const;
	virtual void unpack(UnPacker& p);
	std::string __repr__();
	*/
	~List() {}

	T& extend();

	virtual void pack(Packer& p) const;

	virtual void unpack(UnPacker& p);

	std::string asString() const;

};


template<typename T>
T& List<T>::extend() {
	T x;
	this->push_back(x);
	return this->operator[](this->size()-1);
}

template<typename T>
void List<T>::pack(Packer& p) const {
	typename std::vector<T>::const_iterator a;
	p.pack((int)(this->size()));
	for (a=this->begin(); a!=this->end(); a++)
		p.pack(*a);
}

template<typename T>
void List<T>::unpack(UnPacker& p) {
	T a;
	int size;
	p.unpack(size);
	this->clear();
	this->reserve(size);
	for (int i = 0; i < size; i++) {
		p.unpack(a);
		this->push_back(a);
	}
}

template<typename T>
std::string List<T>::asString() const {
	char repr[128];
	snprintf(repr, 128, "<simdata::List[%d]>", this->size());
	return std::string(repr);
}


NAMESPACE_END // namespace simdata


#endif //__LIST_H__

