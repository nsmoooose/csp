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

#ifndef __CSP_GOOGLEMAPS_TEXTUREIMAGEGENERATOR_H__
#define __CSP_GOOGLEMAPS_TEXTUREIMAGEGENERATOR_H__

#include <csp/cspsim/Projection.h>
#include <csp/tools/googlemaps/cpp/Projection.h>

namespace csp {
namespace googlemaps {

class Progress;

class TextureImageGeneratorError {};
    
class TextureImageGenerator {
public:
    TextureImageGenerator(Progress & progress, csp::Projection const & cspProjection, MercatorProjection const & googleProjection, int oversampling, int textureTileSizeInPixels, double pixelWidth, double pixelHeight);
    virtual ~TextureImageGenerator();
    
    std::string generate(csp::Vector3 const & bottomLeft);
    
    virtual std::string getImageData(MercatorProjection const & projection, MercatorTile const & tile) const = 0;

private:
    Progress & progress_;
    csp::Projection const & cspProjection_;
    MercatorProjection const & googleProjection_;
    int oversampling_;
    int textureTileSizeInPixels_;
    double pixelWidth_;
    double pixelHeight_;
};

}	// end namespace googlemaps
}	// end namespace csp

#endif // __CSP_GOOGLEMAPS_TEXTUREIMAGEGENERATOR_H__
