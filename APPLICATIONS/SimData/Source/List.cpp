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

#include <SimData/List.h>


NAMESPACE_SIMDATA


/*
template<class T> T& List<T>::extend() {
	T x;
	this->push_back(x);
	return this->operator[](this->size()-1);
}

template<class T> void List<T>::pack(Packer& p) const {
	vector<T>::const_iterator a;
	p.pack((int)(this->size()));
	for (a=this->begin(); a!=this->end(); a++)
		p.pack(*a);
}

template<class T> void List<T>::unpack(UnPacker& p) {
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

template<class T> std::string List<T>::__repr__() {
	char repr[128];
	snprintf(repr, 128, "List[%d]", this->size());
	return std::string(repr);
}
*/


NAMESPACE_SIMDATA_END

