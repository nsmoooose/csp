// This file orginated from FlightGear, Modififed by Wolverine


// Stream based logging mechanism.
//
// Written by Bernie Bright, 1998
//
// Copyright (C) 1998  Bernie Bright - bbright@c031.aone.net.au
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA  02111-1307, USA.
//
// $Id: LogStream.cpp,v 1.1 2003/03/18 10:04:05 mkrose Exp $


#include <SimData/LogStream.h>


NAMESPACE_SIMDATA


bool logbuf::logging_enabled = true;

int logbuf::logClass = 0xffffffff;

int logbuf::logPriority = 0;

std::streambuf* logbuf::sbuf = NULL;


logbuf::logbuf()
{
//     if ( sbuf == NULL )
// 	sbuf = std::cerr.rdbuf();
}

logbuf::~logbuf()
{
    if (sbuf) sync();
}

void logbuf::set_sb(std::streambuf* sb)
{
    if (sbuf) sync();
    sbuf = sb;
}

void logbuf::set_log_level(int c, int p)
{
    logClass = c;
    logPriority = p;
}

void logbuf::set_log_classes(int c)
{
    logClass = c;
}

int logbuf::get_log_classes()
{
    return logClass;
}

void logbuf::set_log_priority(int p)
{
    logPriority = p;
}

int logbuf::get_log_priority ()
{
    return logPriority;
}

void logstream::setLogLevels(int c, int p)
{
    logbuf::set_log_level( c, p );
}


NAMESPACE_END // namespace simdata

