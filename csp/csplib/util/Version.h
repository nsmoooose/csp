#pragma once
/* Combat Simulator Project
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @brief Functions and constants related to the version of csplib.
 */

#include <csp/csplib/util/Export.h>

#define CSPLIB_API_VERSION 4

namespace csp {

/**
 * Returns a string representing the csplib version number.  For unversioned
 * releases, this string will include the build date.  No fixed format for the
 * version string has been set yet.
 */
CSPLIB_EXPORT const char *getVersion();

/**
 * Returns a string containing the date and time at which csplib was built.
 */
CSPLIB_EXPORT const char *buildDate();

} // namespace csp


/**
 * @mainpage CSPLIB - Combat Simulator Project Library
 *
 * @section overview Overview
 *
 * CSPLIB is a cross-platform library for that provides infrastructure for
 * the Combat Simulator Project.  The library is written in C++, and provides
 * Python bindings for most functionality via SWIG.  Although developed for
 * use with the Combat Simulator Project (CSP), CSPLIB servers as a general
 * purpose library for simulations.
 *
 * Key features provided by CSPLIB:
 *
 *   @li High-quality data classes for simulations, such as vectors, matrices,
 *       quaternions, dates, geospatial coordinates, and interpolated lookup
 *       tables.
 *   @li Object serialization framework for loading static data into the
 *       simulation at runtime.  Data is stored in a hierarchical set of
 *       files using a simple XML format and precompiled into a compact
 *       binary representation.  The framework allows simulation objects
 *       to be created dynamically at runtime using a data-driven model,
 *       and makes it easy to extend the simulation through third-party
 *       plugin modules.
 *   @li Network communications layer for distributed simulation.  Provides
 *       reliable and unreliable message transport over UDP.  The primary
 *       communications model combines a client-server interface with peer-
 *       to-peer communication between clients.
 *   @li Common utilities such as file path manipulation, endian normalization,
 *       and fixed-size integer types to isolate the simulation from platform
 *       dependencies.
 *   @li Easy to use unit-testing framework.
 *
 * See http://csp.sourceforge.net/wiki for more information or post to
 * http://csp.sourceforge.net/forum if you have suggestions or need help.
 */
