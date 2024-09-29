#pragma once
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
 * @file hash_map.h
 *
 * hash_map portability definitions.
 */

#if defined(__GNUC__) //|| defined(__INTEL_COMPILER)
#  if __GNUC__ >= 3
#    include <ext/hash_map>
#    if __GNUC_MINOR__ > 0 || __GNUC__ > 3
#      define CSP_HASHMAP __gnu_cxx::hash_map
#      define CSP_HASH __gnu_cxx::hash
#    else
#      define CSP_HASHMAP std::hash_map
#      define CSP_HASH std::hash
#    endif
#  else
#    include <hash_map>
#    define CSP_HASHMAP std::hash_map
#    define CSP_HASH std::hash
#  endif
#else
#  ifdef _MSC_VER
#    if defined(_STLPORT)
#      include <hash_map>
#      define CSP_HASHMAP std::hash_map
#      define CSP_HASH std::hash
#    elif ((_MSC_VER <= 1200) && defined(_STLP_WIN32))
#      include <hash_map>
#      define CSP_HASHMAP std::hash_map
#      define CSP_HASH std::hash
#    elif (_MSC_VER == 1300)
#      include <hash_map>
#      define CSP_HASHMAP std::hash_map
#      define CSP_HASH std::hash_compare
#    elif (_MSC_VER > 1300)
#      include <hash_map>
#      define CSP_HASHMAP stdext::hash_map
#      define CSP_HASH stdext::hash_compare
#    endif
#  else
#    error "PLEASE PROVIDE CORRECT #INCLUDE<HASH_MAP> STATEMENT FOR YOUR PLATFORM!"
#  endif
#endif
