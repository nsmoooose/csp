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
 * @file test_File.h
 * @brief Test file utilites.
 */


#include <SimData/FileUtility.h>
#include <SimData/Verify.h>

#include <iostream>
#include <cstdlib>


using namespace simdata::ospath;

void test() {

	// test directory (minimal!)
	DirectoryContents dc = getDirectoryContents(".");
	assert(dc.size() > 0);

	// test file extension
	SIMDATA_VERIFY_EQ(getFileExtension("/foo/bar.baz"), "baz");
	SIMDATA_VERIFY_EQ(getFileExtension("\\foo\\bar.baz"), "baz");
	SIMDATA_VERIFY_EQ(getFileExtension("/foo/bar.baz.bif"), "bif");
	SIMDATA_VERIFY_EQ(getFileExtension("\\foo\\bar.baz.bif"), "bif");
	SIMDATA_VERIFY_EQ(getFileExtension("/foo.dir/bar"), "");
	SIMDATA_VERIFY_EQ(getFileExtension("\\foo.dir\\bar"), "");
	SIMDATA_VERIFY_EQ(getFileExtension("/foo.dir/bar.x"), "x");
	SIMDATA_VERIFY_EQ(getFileExtension("\\foo.dir\\bar.x"), "x");
	SIMDATA_VERIFY_EQ(getFileExtension("bar.x"), "x");
	SIMDATA_VERIFY_EQ(getFileExtension("/bar.x"), "x");
	SIMDATA_VERIFY_EQ(getFileExtension("\\bar.x"), "x");
	SIMDATA_VERIFY_EQ(getFileExtension("bar"), "");
	SIMDATA_VERIFY_EQ(getFileExtension("/bar"), "");
	SIMDATA_VERIFY_EQ(getFileExtension("\\bar"), "");
	SIMDATA_VERIFY_EQ(getFileExtension(""), "");

	// test file extension stripping
	std::string path;
	path = "/foo/bar.baz";
	SIMDATA_VERIFY_EQ(stripFileExtension(path), "baz");
	SIMDATA_VERIFY_EQ(path, "/foo/bar");
	path = "\\foo\\bar.baz";
	SIMDATA_VERIFY_EQ(stripFileExtension(path), "baz");
	SIMDATA_VERIFY_EQ(path, "\\foo\\bar");
	path = "/foo/bar.baz.bif";
	SIMDATA_VERIFY_EQ(stripFileExtension(path), "bif");
	SIMDATA_VERIFY_EQ(path, "/foo/bar.baz");
	path = "foo.bif";
	SIMDATA_VERIFY_EQ(stripFileExtension(path), "bif");
	SIMDATA_VERIFY_EQ(path, "foo");
	path = "foobar";
	SIMDATA_VERIFY_EQ(stripFileExtension(path), "");
	SIMDATA_VERIFY_EQ(path, "foobar");
	path = ".foobar";
	SIMDATA_VERIFY_EQ(stripFileExtension(path), "foobar");
	SIMDATA_VERIFY_EQ(path, "");
	path = "";
	SIMDATA_VERIFY_EQ(stripFileExtension(path), "");
	SIMDATA_VERIFY_EQ(path, "");

	std::cout << "OK\n";
	::exit(0);
}

int main() {
	test();
	return 0;
};

