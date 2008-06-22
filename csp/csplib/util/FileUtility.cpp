/* Combat Simulator Project
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
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
 * @file FileUtility.cpp
 *
 * Platform independent file utilities.
 */


#include <csp/csplib/util/FileUtility.h>

#include <cstdlib>
#include <string.h>

#ifdef _WIN32
#  define STRICT  /* Strict typing, please */
#  define NOMINMAX
#  include <windows.h>
#  include <direct.h>
#  include <ctype.h>
#  ifdef _MSC_VER
#    include <io.h>
#  endif /* _MSC_VER */
#else /* _WIN32 */
#  include <unistd.h>
#  include <sys/types.h>
#  include <dirent.h>
#endif



CSP_NAMESPACE


#ifdef _WIN32
	const char ospath::DIR_SEPARATOR = '\\';
	const char ospath::PATH_SEPARATOR = ';';
#else
	const char ospath::DIR_SEPARATOR = '/';
	const char ospath::PATH_SEPARATOR = ':';
#endif


std::string ospath::basename(const std::string &path)
{
	std::string::size_type base;
	base = path.rfind(DIR_SEPARATOR);
	if (base != std::string::npos) {
		return path.substr(base+1);
	}
#ifdef _WIN32
	if (path.length() > 1 && isalpha(path[0]) && path[1] == ':') {
		return path.substr(2);
	}
#endif // _WIN32
	return path;
}

bool ospath::isabs(const std::string &path)
{
	int n = path.length();
	if (n==0) {
		return false;
	}
	if (path[0] == DIR_SEPARATOR) {
		return true;
	}
#ifdef _WIN32
	if (n > 2 && isalpha(path[0]) && path[1] == ':' && path[2] == DIR_SEPARATOR) {
		return TRUE;
	}
#endif // _WIN32
	return false;
}

std::string ospath::skiproot(const std::string &path) {
	int n = path.length();
	if (n > 0 && path[0] == DIR_SEPARATOR) {
		return path.substr(1);
	}
#ifdef _WIN32
	if (n > 2 && isalpha(path[0]) && path[1] == ':' && path[2] == DIR_SEPARATOR) {
		return path.substr(3);
	}
#endif // _WIN32
	return path;
}

std::string ospath::dirname(const std::string &path) {
	std::string::size_type base = path.rfind(DIR_SEPARATOR);
	if (base == std::string::npos) {
		return ".";
	}
	return path.substr(0, base);
}

std::string ospath::currentdir() {
	std::string dir;
	dir = DIR_SEPARATOR;
	char *buffer = getcwd(0, 0);
	if (buffer) {
		dir = buffer;
		free(buffer);
	}
	return dir;
}

std::string ospath::ext(const std::string &path) {
	std::string::size_type ext;
	ext = path.find_last_of(".");
	if (ext != std::string::npos && path[ext]=='.') {
		return path.substr(ext);
	}
	return "";
}

std::string ospath::join(const std::string &a, const std::string &b) {
	std::string result;
	std::string::size_type idx;
	idx = a.length();
	if (idx > 0 && (a[idx-1] == '/' || a[idx-1] == '\\')) idx--;
	result = a.substr(0, idx);
	return result + DIR_SEPARATOR + skiproot(b);
}

std::string ospath::stringreplace(const std::string &path, char search, char replace) {
	if (search == replace) return path;
	std::string result;
	char *str = strdup(path.c_str());
	char *idx = str;
	for (; *idx; idx++) {
		if (*idx == search) *idx = replace;
	}
	result = str;
	free(str);
	return result;
}

std::string const &ospath::addpath(std::string &pathlist, const std::string &path) {
	if (pathlist.size() > 0) pathlist = pathlist + PATH_SEPARATOR;
	pathlist = pathlist + path;
	return pathlist;
}

std::string ospath::normalize(const std::string &path) {
	return stringreplace(path, '\\', '/');
}

std::string ospath::denormalize(const std::string &path) {
	return stringreplace(path, '/', DIR_SEPARATOR);
}

std::string ospath::filter(const std::string &path) {
	return denormalize(normalize(path));
}

std::string ospath::stripFileExtension(std::string &path) {
	std::string ext = getFileExtension(path);
	if (!ext.empty()) path = path.substr(0, path.size() - ext.size() - 1);
	return ext;
}

std::string ospath::getFileExtension(const std::string &path) {
	std::string::size_type sep_idx0 = path.rfind('/');
	std::string::size_type sep_idx1 = path.rfind('\\');
	if (sep_idx0 == std::string::npos) sep_idx0 = 0;
	if (sep_idx1 == std::string::npos) sep_idx1 = 0;
	std::string::size_type sep_idx = std::max(sep_idx0, sep_idx1);
	std::string::size_type idx = path.rfind('.');
	if (idx == std::string::npos || idx < sep_idx) return "";
	return path.substr(idx+1);
}

bool ospath::exists(const std::string &path) {
#ifdef _WIN32
	static const int mode = 0; // existence only
#else // POSIX (hopefully)
	static const int mode = F_OK;
#endif
	return access(path.c_str(), mode) == 0;
}

ospath::DirectoryContents ospath::getDirectoryContents(std::string const &path) {
	DirectoryContents entries;
#ifdef _WIN32
	WIN32_FIND_DATA ffd;
	std::string search_path = path + "\\*";
	HANDLE handle = FindFirstFile(search_path.c_str(), &ffd);
	if (handle != INVALID_HANDLE_VALUE) {
		do {
			entries.push_back(ffd.cFileName);
		} while (FindNextFile(handle, &ffd) != 0);
		FindClose(handle);
	}
#else // POSIX (hopefully)
	DIR *dir = opendir(path.c_str());
	if (dir != NULL) {
		struct dirent *entry;
		while ((entry=readdir(dir)) != NULL) {
			entries.push_back(entry->d_name);
		}
		closedir(dir);
	}
#endif
	return entries;
}

CSP_NAMESPACE_END

