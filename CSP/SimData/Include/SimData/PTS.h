/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
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
 * @file PTS.h
 *
 * Simulated partial template specialization and template metaprogramming
 * support.
 *
 */


#ifndef __SIMDATA_PTS_H__
#define __SIMDATA_PTS_H__

#include <SimData/Namespace.h>


// is partial template specialization supported?
#if defined(_MSC_VER) && (_MSC_VER <= 1300)
	#define __SIMDATA_PTS_SIM
#endif


NAMESPACE_SIMDATA


/** Template meta-programming structures.
 */
namespace meta {

struct SelectThen
{       template<class Then, class Else>
	struct Result { typedef Then RET; };
};

struct SelectElse
{       template<class Then, class Else>
	struct Result { typedef Else RET; };
};

template<bool Condition>
struct Selector { typedef SelectThen RET; };

template<>
struct Selector<false> { typedef SelectElse RET; };

template<bool Condition, class Then, class Else>
struct IF
{	typedef typename Selector<Condition>::RET select;
	typedef typename select::Result<Then,Else>::RET RET;
};

} // meta


NAMESPACE_SIMDATA_END

#endif // __SIMDATA_PTS_H__


