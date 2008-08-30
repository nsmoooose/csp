// CSPLayout - Copyright 2003-2005 Mark Rose <mkrose@users.sourceforge.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, US

#ifndef __CSP_GOOGLEMAPS_PROGRESS_H__
#define __CSP_GOOGLEMAPS_PROGRESS_H__

namespace csp {
namespace googlemaps {

class Progress {
public:
	Progress();
    virtual ~Progress();
    
    virtual bool mustCancel() = 0;
    virtual void setCurrentTilePercent(unsigned int percent) = 0;
};

}	// end namespace googlemaps
}	// end namespace csp

#endif // __CSP_GOOGLEMAPS_PROGRESS_H__
