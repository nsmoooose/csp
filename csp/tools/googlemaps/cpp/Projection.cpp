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

#include <csp/tools/googlemaps/cpp/Projection.h>
#include <cmath>

namespace csp {
namespace googlemaps {

MercatorTile::MercatorTile(int aColumn, int aRow)
    : column( aColumn )
    , row( aRow )
{
}

bool MercatorTile::operator<(MercatorTile const & rhs) const {
    if ( row < rhs.row ) return true;
    if ( row > rhs.row ) return false;
    if ( column < rhs.column ) return true;
    return false;
}


MercatorTilePixel::MercatorTilePixel( MercatorTile aTile, Pixel pixelInTile )
    : tile( aTile )
    , pixel( pixelInTile )
{
}

MercatorTilePixel::MercatorTilePixel( std::div_t const & column, std::div_t const & row )
    : tile( column.quot, row.quot )
    , pixel( column.rem, row.rem )
{
}


double const MercatorProjection::PI = 3.14159265358979323846;

MercatorProjection::MercatorProjection(int aZoomLevel)
    : zoomLevel( aZoomLevel )
    , nbTiles( int( std::pow(2.0, zoomLevel) ) )
    , circumference( tileSize * nbTiles )
    , radius( circumference / (2.0 * PI) )
{
}

double MercatorProjection::scaleAt(double latitude) const {
    return GeoRef::WGS84.R * std::cos(latitude) / radius;
}

int MercatorProjection::boundPixelX(int x) const {
    if ( x < 0 ) x += circumference;
    if (x >= circumference) x -= circumference;
    return x;
}

int MercatorProjection::boundPixelY(int y) const {
    if ( y < 0 ) y = 0;
    if ( y >= circumference ) y = circumference - 1;
    return y;
}

int MercatorProjection::longitudeToPixelX(double longitude) const {
    return int( longitude * radius + circumference / 2.0 );
}

int MercatorProjection::latitudeToPixelY(double latitude) const {
    double sinLat = std::sin( latitude );
    return int( ( circumference - radius * std::log( (1.0 + sinLat) / (1.0 - sinLat) ) ) / 2.0 );
}

Pixel MercatorProjection::llaToPixel(csp::LLA const & lla) const {
    return Pixel( longitudeToPixelX( lla.longitude() ), latitudeToPixelY( lla.latitude() ) );
}

MercatorTilePixel MercatorProjection::pixelToTilePixel(Pixel const & pixel) const {
    return MercatorTilePixel( std::div( boundPixelX(pixel.x), tileSize ), std::div( boundPixelY(pixel.y), tileSize ) );
}

MercatorTilePixel MercatorProjection::llaToTilePixel(csp::LLA const & lla) const {
    return pixelToTilePixel( llaToPixel( lla ) );
}

}	// end namespace googlemaps
}	// end namespace csp
