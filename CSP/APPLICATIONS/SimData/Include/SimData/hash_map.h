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
 * @file hash_map.h
 *
 * hash_map portability definitions.
 */

#ifndef __SIMDATA_HASH_MAP_H__
#define __SIMDATA_HASH_MAP_H__

#ifdef __GNUC__
  #if __GNUC__ == 3
    #include <ext/hash_map>
    #if __GNUC_MINOR__ > 0
	#define HASH_MAP __gnu_cxx::hash_map
        #define HASH __gnu_cxx::hash
    #else
	#define HASH_MAP std::hash_map
	#define HASH std::hash
    #endif
  #else
    #include <hash_map>
    #define HASH_MAP std::hash_map
    #define HASH std::hash
  #endif
#else
  #ifdef _MSC_VER 
   #if (_MSC_VER <= 1200) && defined(_STLP_WIN32)
    #include <hash_map>
    #define HASH_MAP std::hash_map
    #define HASH std::hash
  #elif (_MSC_VER == 1300) 
    #include <hash_map>
    #define HASH_MAP std::hash_map
    #define HASH std::hash_compare
  #endif
  #else
    #error "PLEASE PROVIDE CORRECT #INCLUDE<HASH_MAP> STATEMENT FOR YOUR PLATFORM!"
  #endif
#endif

template <class key, class val, class hash, class eq> struct HASH_MAPS {
#if defined(_MSC_VER) && (_MSC_VER == 1300)
	typedef HASH_MAP<key, val, eq> Type;
#else
	typedef HASH_MAP<key, val, hash, eq> Type;
#endif
};

#endif // __SIMDATA_HASH_MAP_H__