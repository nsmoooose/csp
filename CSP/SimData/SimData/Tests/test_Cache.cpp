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
 * @file test_Cache.h
 * @brief Test MRU cache.
 */


#include <SimData/Cache.h>
#include <SimData/Verify.h>

#include <string>
#include <iostream>
#include <cstdlib>

USING_SIMDATA

void test() {
	MostRecentlyUsedCache<int, std::string> cache(5);
	std::string value;

	cache.insert(8, "is enough");
	cache.insert(3, "is company");
	cache.insert(7, "lucky");
	cache.insert(100, "years in a century");
	cache.insert(42, "The Answer to Life, the Universe, and Everything");
	cache.insert(10, "fingers"); // now 6 element, so last used (8) is dropped

	SIMDATA_VERIFY_EQ(cache.find(8, value), false);   // 8 not in cache
	SIMDATA_VERIFY_EQ(cache.find(7, value), true);    // fast hash_map find -- found
	SIMDATA_VERIFY_EQ(value, "lucky");
	cache.insert(4, "quarters");                        // last used (3) is dropped
	SIMDATA_VERIFY_EQ(cache.find(3, value), false);   // 3 not in cache
	cache.insert(52, "weeks");                          // last used (100) is dropped
	SIMDATA_VERIFY_EQ(cache.find(7, value), true);    // 7 still in cache!
	SIMDATA_VERIFY_EQ(value, "lucky");
	SIMDATA_VERIFY_EQ(cache.find(100, value), false); // 100 not in cache

	for (int i = 0; i < 10; ++i) {
		cache.insert(7, "lucky");
	}
	SIMDATA_VERIFY_EQ(cache.find(7, value), true);
	SIMDATA_VERIFY_EQ(value, "lucky");
	SIMDATA_VERIFY_EQ(cache.find(42, value), true);
	SIMDATA_VERIFY_EQ(value, "The Answer to Life, the Universe, and Everything");

	std::cout << "OK\n";
	::exit(0);
}

int main() {
	test();
	return 0;
}

