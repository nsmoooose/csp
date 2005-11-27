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

%module Log
%{
#include <csp/csplib/util/LogStream.h>
#include <csp/csplib/util/Log.h>
%}

%include "csp/csplib/util/Namespace.h"
%import "csp/csplib/util/LogConstants.h"

CSP_NAMESPACE

class LogStream
{
public:
	enum { cTerse=0, cPriority=1, cDatestamp=2, cTimestamp=4, cLinestamp=8, cFullPath=16, cThread=32, cVerbose=~0 };
	enum { cDebug=0, cInfo=1, cWarning=2, cError=3, cFatal=4 };

	LogStream(std::ostream& out);
	~LogStream();
	void logToFile(std::string const &fn);
	void setFlags(int);
	void setPriority(int);
	void setCategories(int);

%extend {
#define CSP_LOG_PYTHON(P) \
	if (self->isNoteworthy(CSPLOG_PRIORITY(P), CSPLOG_CATEGORY(ALL))) \
		CSP(LogStream::LogEntry)(*self, CSPLOG_PRIORITY(P))

	void debug(const char *msg) { CSP_LOG_PYTHON(DEBUG) << msg; }
	void info(const char *msg) { CSP_LOG_PYTHON(INFO) << msg; }
	void warning(const char *msg) { CSP_LOG_PYTHON(WARNING) << msg; }
	void error(const char *msg) { CSP_LOG_PYTHON(ERROR) << msg; }
	void fatal(const char *msg) { CSP_LOG_PYTHON(FATAL) << msg; }
#undef CSP_LOG_PYTHON
}

};

CSP_NAMESPACE_END

%include "csp/csplib/util/LogConstants.h"
%include "csp/csplib/util/Log.h"

