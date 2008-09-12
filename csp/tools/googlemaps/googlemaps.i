// Combat Simulator Project
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

%module(directors="1") googlemaps_module
%feature("director");

%{
#include "csp/tools/googlemaps/cpp/Pixel.h"
#include "csp/tools/googlemaps/cpp/Progress.h"
#include "csp/tools/googlemaps/cpp/Projection.h"
#include "csp/tools/googlemaps/cpp/TextureImageGenerator.h"
%}

%include "std_string.i"

%extend csp::googlemaps::MercatorProjection {
%pythoncode %{
    zoomLevels = range(zoomLevelBegin, zoomLevelEnd)
    
    def tileUrl(self, tile, googlemapsServer):
        urlSuffix = self.tileUrlSuffix( tile )
        url = googlemapsServer % ( (tile.column + tile.row) % 4 ) + MercatorProjectionUrlPrefix + urlSuffix
        fileName = urlSuffix + ".jpg"
        return url, fileName
%}
};

%ignore csp::googlemaps::MercatorTilePixel::MercatorTilePixel( std::div_t const & column, std::div_t const & row );

%exception csp::googlemaps::TextureImageGenerator::generate(csp::Vector3 const & bottomLeft) {
    try {
        $action
    }
    catch ( csp::googlemaps::TextureImageGeneratorError & ) {
        PyErr_SetString(PyExc_RuntimeError, "csp::googlemaps::TextureImageGeneratorError");
        return 0;
    }
}

%include "csp/tools/googlemaps/cpp/Pixel.h"
%include "csp/tools/googlemaps/cpp/Progress.h"
%include "csp/tools/googlemaps/cpp/Projection.h"
%include "csp/tools/googlemaps/cpp/TextureImageGenerator.h"
