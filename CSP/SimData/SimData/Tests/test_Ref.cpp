/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file test_Ref.h
 * @brief Test reference counting.
 */


#include <SimData/Ref.h>

#include <iostream>
#include <cstdlib>
#include <cassert>

int count = 0;

struct R: public simdata::Referenced {
	R() { ++count; }
	~R() { --count; }
};

typedef simdata::Ref<R> Ref;

void test() {
	{
		assert(count == 0);
		Ref x = new R;
		assert(count == 1);
		{
			Ref y = x;
			assert(count == 1);
		}
		assert(count == 1);
		{
			Ref y = new R;
			assert(count == 2);
			y = x;
			assert(count == 1);
			y = new R;
			assert(count == 2);
		}
		assert(count == 1);
	}
	assert(count == 0);
	std::cout << "OK\n";
	::exit(0);
}

int main() {
	test();
	return 0;
};

