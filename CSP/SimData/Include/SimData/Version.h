/* SimData: Data Infrastructure for Simulations
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file Version.h
 * @brief Functions and constants related to the version of SimData.
 */

#ifndef __SIMDATA_VERSION_H__
#define __SIMDATA_VERSION_H__

#include <SimData/Namespace.h>

#define SIMDATA_API_VERSION 4

NAMESPACE_SIMDATA

/**
 * Returns a string representing the SimData version
 * number.  For unversioned releases, this string will
 * include the build date.  No fixed format for the
 * version string has been set yet.
 */
const char *getVersion();

/**
 * Returns a string containing the date and time at
 * which SimData was built.
 */
const char *buildDate();

NAMESPACE_SIMDATA_END


/**
 * @mainpage SimData - Data Infrastructure for Simulations
 *
 * @section overview Overview
 *
 * SimData is a cross-platform, independent library for data management in
 * simulations. The library is written in C++, and provides full binding to
 * Python via SWIG. It was developed for use with the Combat Simulator
 * Project (CSP), but is meant to be a general purpose library. SimData
 * serves two interrelated purposes. The first is to provide high-quality
 * data classes that are useful in simulation environments. Examples
 * include vectors, matrices, quaternions, dates, geospacial coordinates,
 * and interpolated lookup tables. The other purpose is to allow static
 * external data to be quickly and easily loaded into the simulation at
 * runtime. Data is stored in a hierarchical set of files using a
 * simple XML format. These files are then precompiled into a binary format
 * for use at runtime. Using the SimData facilities, objects that rely on
 * external data can be created dynamically at runtime with all the correct
 * data automatically and transparently loaded. As a side benefit, when
 * combined with the dynamic module loading capability of Python, the
 * object creation capability of SimData provides a very simple way to
 * extend the simulation functionality using third-party plugin components.
 *
 * See http://csp.sourceforge.net/wiki/SimData for more information
 * or post to http://csp.sourceforge.net/forum if you have suggestions
 * or need help.
 *
 */


#endif // __SIMDATA_VERSION_H__

