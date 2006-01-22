/* Combat Simulator Project
 * Copyright (C) 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file test_FileUtility.h
 * @brief Test file utilites.
 */


#include <csp/csplib/util/FileUtility.h>
#include <csp/csplib/util/Testing.h>

using namespace csp::ospath;

CSP_TESTFIXTURE(FileUtility) {

	CSP_TESTCASE(PathManipulation) {
		// test directory (minimal!)
		DirectoryContents dc = getDirectoryContents(".");
		assert(dc.size() > 0);

		// test file extension
		CSP_VERIFY_EQ(getFileExtension("/foo/bar.baz"), "baz");
		CSP_VERIFY_EQ(getFileExtension("\\foo\\bar.baz"), "baz");
		CSP_VERIFY_EQ(getFileExtension("/foo/bar.baz.bif"), "bif");
		CSP_VERIFY_EQ(getFileExtension("\\foo\\bar.baz.bif"), "bif");
		CSP_VERIFY_EQ(getFileExtension("/foo.dir/bar"), "");
		CSP_VERIFY_EQ(getFileExtension("\\foo.dir\\bar"), "");
		CSP_VERIFY_EQ(getFileExtension("/foo.dir/bar.x"), "x");
		CSP_VERIFY_EQ(getFileExtension("\\foo.dir\\bar.x"), "x");
		CSP_VERIFY_EQ(getFileExtension("bar.x"), "x");
		CSP_VERIFY_EQ(getFileExtension("/bar.x"), "x");
		CSP_VERIFY_EQ(getFileExtension("\\bar.x"), "x");
		CSP_VERIFY_EQ(getFileExtension("bar"), "");
		CSP_VERIFY_EQ(getFileExtension("/bar"), "");
		CSP_VERIFY_EQ(getFileExtension("\\bar"), "");
		CSP_VERIFY_EQ(getFileExtension(""), "");

		// test file extension stripping
		std::string path;
		path = "/foo/bar.baz";
		CSP_VERIFY_EQ(stripFileExtension(path), "baz");
		CSP_VERIFY_EQ(path, "/foo/bar");
		path = "\\foo\\bar.baz";
		CSP_VERIFY_EQ(stripFileExtension(path), "baz");
		CSP_VERIFY_EQ(path, "\\foo\\bar");
		path = "/foo/bar.baz.bif";
		CSP_VERIFY_EQ(stripFileExtension(path), "bif");
		CSP_VERIFY_EQ(path, "/foo/bar.baz");
		path = "foo.bif";
		CSP_VERIFY_EQ(stripFileExtension(path), "bif");
		CSP_VERIFY_EQ(path, "foo");
		path = "foobar";
		CSP_VERIFY_EQ(stripFileExtension(path), "");
		CSP_VERIFY_EQ(path, "foobar");
		path = ".foobar";
		CSP_VERIFY_EQ(stripFileExtension(path), "foobar");
		CSP_VERIFY_EQ(path, "");
		path = "";
		CSP_VERIFY_EQ(stripFileExtension(path), "");
		CSP_VERIFY_EQ(path, "");
	}
};

