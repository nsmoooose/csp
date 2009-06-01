/* Combat Simulator Project
 * Copyright 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file WeakReferenced.cpp
 */

#include <csp/csplib/util/WeakReferenced.h>

namespace csp {

ReferenceValidator::ReferenceValidator() : counter(0), _valid(true) {
}

bool ReferenceValidator::valid() const {
	return _valid;
}

void ReferenceValidator::reset() {
	_valid = false;
}

void ReferenceValidator::incref() {
	counter++;
}

void ReferenceValidator::decref() {
	counter--;
	if(counter <= 0) {
		delete this;
	}
}

WeakReferencedBase::WeakReferencedBase() : referenceValidator(0) {
}
	
WeakReferencedBase::~WeakReferencedBase() {
	if(referenceValidator != 0) {
		referenceValidator->reset();
		referenceValidator->decref();
	}
}
	
ReferenceValidator* WeakReferencedBase::getReferenceValidator() {
	if(referenceValidator == 0) {
		referenceValidator = new ReferenceValidator();
		referenceValidator->incref();
	}

	// Increase the reference count for the calling class.	
	referenceValidator->incref();
	return referenceValidator;
}

} // namespace csp
