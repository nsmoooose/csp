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

#include <csp/tools/googlemaps/cpp/TextureImageGenerator.h>
#include <csp/tools/googlemaps/cpp/Progress.h>
#include <map>

#include <iostream> // TODO: remove after debug

namespace csp {
namespace googlemaps {


class Color {
public:
    Color()
        : red(0), green(0), blue(0)
    {
    }
    
    void operator/=(unsigned int rhs) {
        red /= rhs;
        green /= rhs;
        blue /= rhs;
    }
    
    unsigned int red;
    unsigned int green;
    unsigned int blue;
};

class Image {
public:
    Image()
        : width( 0 )
        , height( 0 )
        , data( 0 )
    {
    }
    
    Image(int aWidth, int aHeight)
        : width( aWidth )
        , height( aHeight )
        , data( new unsigned char[ dataSize() ] )
    {
    }
    
    ~Image() {
        if ( data ) delete data;
    }
    
    size_t dataSize() const {
        return width * height * nbColorPlane;
    }
    
    void setData(int aWidth, int aHeight, std::string const & newData) {
        if ( data ) delete data;
        width = aWidth;
        height = aHeight;
        data = new unsigned char[ dataSize() ];
        newData.copy( reinterpret_cast<char *>(data), dataSize() );
    }
    
    void setColor(int x, int y, Color const & color) {
        unsigned char * pixel = &data[ (y * width + x) * nbColorPlane ];
        pixel[ red ] = color.red;
        pixel[ green ] = color.green;
        pixel[ blue ] = color.blue;
    }
    
    void addToColor(int x, int y, Color & color) {
        unsigned char * pixel = &data[ (y * width + x) * nbColorPlane ];
        color.red += pixel[ red ];
        color.green += pixel[ green ];
        color.blue += pixel[ blue ];
    }
    
    std::string asString() const {
        return std::string( reinterpret_cast<const char *>(data), dataSize() );
    }
    
    int width;
    int height;
    unsigned char * data;
    enum { red = 0, green, blue, nbColorPlane };
};


class GoogleImage;

class GoogleImages {
public:
    GoogleImages(TextureImageGenerator const & generator);
    ~GoogleImages();
    
    GoogleImage & get(MercatorTile const & tile, MercatorProjection const & googleProjection);

private:
    typedef std::map<MercatorTile, GoogleImage *> Map;
    Map map;
    TextureImageGenerator const & generator_;
};


class GoogleImage {
public:
    GoogleImage(MercatorTile const & tile, MercatorProjection const & googleProjection, TextureImageGenerator const & generator, GoogleImages & googleImages)
        : downLevelGoogleImage_( 0 )
    {
        std::string imageData = generator.getImageData(googleProjection, tile);
        
        if ( imageData.empty() ) {
            int zoomLevel = googleProjection.zoomLevel - 1;
            if ( zoomLevel < MercatorProjection::zoomLevelBegin ) throw TextureImageGeneratorError();
            MercatorProjection downLevelProjection( zoomLevel );
            
            std::div_t halfColumn = std::div( tile.column, 2 );
            std::div_t halfRow = std::div( tile.row, 2 );
            
            MercatorTile downLevelTile( halfColumn.quot, halfRow.quot );
            downLevelGoogleImage_ = &googleImages.get( downLevelTile, downLevelProjection );
            
            downLevelOffsetColumn_ = halfColumn.rem * MercatorProjection::tileSize / 2;
            downLevelOffsetRow_ = halfRow.rem * MercatorProjection::tileSize / 2;
        }
        else {
            image_.setData( MercatorProjection::tileSize, MercatorProjection::tileSize, imageData );
        }
    }
    
    void addToColor(int x, int y, Color & color) {
        if ( downLevelGoogleImage_ ) {
            downLevelGoogleImage_->addToColor( downLevelOffsetColumn_ + x / 2, downLevelOffsetRow_ + y / 2, color );
        }
        else {
            image_.addToColor( x, y, color );
        }
    }
    
    GoogleImage * downLevelGoogleImage_;
    Image image_;
    int downLevelOffsetColumn_;
    int downLevelOffsetRow_;
};


GoogleImages::GoogleImages(TextureImageGenerator const & generator)
    : generator_( generator )
{
}

GoogleImages::~GoogleImages() {
    for ( Map::iterator it = map.begin(); it != map.end(); ++it ) {
        delete it->second;
    }
}

GoogleImage & GoogleImages::get(MercatorTile const & tile, MercatorProjection const & googleProjection) {
    std::pair<Map::iterator, bool> insertResult = map.insert( Map::value_type( tile, 0 ) );
    
    if ( insertResult.second ) {
        insertResult.first->second = new GoogleImage(tile, googleProjection, generator_, *this);
    }
    
    return *insertResult.first->second;
}


TextureImageGenerator::TextureImageGenerator(Progress & progress, csp::Projection const & cspProjection, MercatorProjection const & googleProjection, int oversampling, int textureTileSizeInPixels, double pixelWidth, double pixelHeight)
    : progress_( progress )
    , cspProjection_( cspProjection )
    , googleProjection_( googleProjection )
    , oversampling_( oversampling )
    , textureTileSizeInPixels_( textureTileSizeInPixels )
    , pixelWidth_( pixelWidth )
    , pixelHeight_( pixelHeight )
{
}

TextureImageGenerator::~TextureImageGenerator() {
}

std::string TextureImageGenerator::generate(csp::Vector3 const & bottomLeft) {
    int oversampling2 = oversampling_ * oversampling_;
    
    int const nbColorPlane = 3;
    Image image( textureTileSizeInPixels_, textureTileSizeInPixels_ );
    
    GoogleImages googleImages(*this);
    
    for ( int y = 0; y != textureTileSizeInPixels_; ++y ) {
        progress_.setCurrentTilePercent( 100 * y / textureTileSizeInPixels_ );
        if ( progress_.mustCancel() ) return "";
        
        for ( int x = 0; x != textureTileSizeInPixels_; ++x ) {
            Color color;
            
            for ( int subY = 0; subY != oversampling_; ++subY ) {
                for ( int subX = 0; subX != oversampling_; ++subX ) {
                    csp::Vector3 position = bottomLeft;
                    position.x() += (x * oversampling_ + subX) * pixelWidth_;
                    position.y() += (y * oversampling_ + subY) * pixelHeight_;
                    
                    MercatorTilePixel tilePixel = googleProjection_.llaToTilePixel( cspProjection_.convert( position ) );
                    
                    GoogleImage & googleImage = googleImages.get( tilePixel.tile, googleProjection_ );
                    googleImage.addToColor( tilePixel.pixel.x, tilePixel.pixel.y, color );
                }
            }
            
            color /= oversampling2;
            image.setColor( x, y, color );
        }
    }
    
    return image.asString();
}

}	// end namespace googlemaps
}	// end namespace csp
