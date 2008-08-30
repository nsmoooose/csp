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

#ifndef __CSP_GOOGLEMAPS_PROJECTION_H__
#define __CSP_GOOGLEMAPS_PROJECTION_H__

#include <csp/csplib/data/GeoPos.h>
#include <csp/tools/googlemaps/cpp/Pixel.h>

namespace csp {
namespace googlemaps {

class MercatorTile {
public:
    MercatorTile(int aColumn = 0, int aRow = 0);
    
    bool operator<(MercatorTile const & rhs) const;
    
    int column;
    int row;
};

class MercatorTilePixel {
public:
    MercatorTilePixel( MercatorTile aTile = MercatorTile(), Pixel pixelInTile = Pixel() );
    MercatorTilePixel( std::div_t const & column, std::div_t const & row );
    
    MercatorTile tile;
    Pixel pixel;
};

#define MercatorProjectionUrlPrefix "/kh?v=30&t="

class MercatorProjection {
public:
	MercatorProjection(int aZoomLevel);
    
    double scaleAt(double latitude) const;
    
    int boundPixelX(int x) const;
    int boundPixelY(int y) const;
    int longitudeToPixelX(double longitude) const;
    int latitudeToPixelY(double latitude) const;
    Pixel llaToPixel(csp::LLA const & lla) const;
    MercatorTilePixel pixelToTilePixel(Pixel const & pixel) const;
    MercatorTilePixel llaToTilePixel(csp::LLA const & lla) const;
    std::string tileUrlSuffix(MercatorTile const & tile) const;
    
    int const zoomLevel;
    int const nbTiles;
    int const circumference;
    double const radius;
    
    enum {
        tileSize = 256,
        zoomLevelBegin = 0,
        zoomLevelEnd = 21
    };

private:
    static double const PI;
};

}	// end namespace googlemaps
}	// end namespace csp

#endif // __CSP_GOOGLEMAPS_PROJECTION_H__
