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
 * @file test_CircularBuffer.cpp
 * @brief Test the CircularBuffer class.
 */


#include <SimData/CircularBuffer.h>
#include <SimData/Trace.h>

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <deque>

#define TEST_LOOP_COUNT 1000000
#define DBG(x)

using namespace simdata;


void testOneByte() {
	std::cerr << "RUNNING ONE BYTE TEST\n";
	CircularBuffer b(433);
	assert(b.getAllocatedSpace() == 0);
	assert(b.getMaximumAllocation() > 430);
	for (int i = 0; i < TEST_LOOP_COUNT; ++i) {
		DBG(std::cerr << "iteration: " << i << "\n";)
		uint8 *x = b.getWriteBuffer(1);
		*x = static_cast<uint8>(i & 255);
		b.commitWriteBuffer();
		uint32 size;
		x = b.getReadBuffer(size);
		assert(size == 1);
		assert(*x == (i & 255));
		b.releaseReadBuffer();
	}
	assert(b.getAllocatedSpace() == 0);
}


void testRandomUse() {
	std::cerr << "RUNNING RANDOM USE TEST\n";
	CircularBuffer b(1024);
	std::deque<uint8*> q_ptr;
	std::deque<uint32> q_size;
	for (int i = 0; i < TEST_LOOP_COUNT; ++i) {
		DBG(std::cerr << "iteration: " << i << "\n";)
		if (rand() & 1) {
			uint32 size;
			uint8 *x = b.getReadBuffer(size);
			if (x != 0) {
				DBG(std::cerr << "read  " << (void*)x << " " << size << "\n";)
				assert(x == q_ptr.front());
				q_ptr.pop_front();
				assert(size == q_size.front());
				q_size.pop_front();
				b.releaseReadBuffer();
			} else {
				assert(q_ptr.empty());
				DBG(std::cerr << "read --- empty!\n";)
			}
		} else {
			uint32 size = (rand() & 255);
			uint8 *x = b.getWriteBuffer(size);
			if (x != 0) {
				DBG(std::cerr << "write " << (void*)x << " " << size << "\n";)
				q_ptr.push_back(x);
				q_size.push_back(size);
				b.commitWriteBuffer();
			} else {
				DBG(std::cerr << "write --- full! " << q_ptr.size() << "\n";)
			}
		}
	}
}

void test() {
	testRandomUse();
	testOneByte();
	std::cout << "OK\n";
	::exit(0);
}


int main() {
	AutoTrace::install();
	test();
	return 0;
}

