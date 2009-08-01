#include <sstream>
#include <osgDB/Registry>
#include <osgDB/FileNameUtils>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>
#include <osg/Texture2D>
#include <osg/PositionAttitudeTransform>
#include <csp/tools/layout2/cpp/OsgTerrainOverviewManipulator.h>
#include <csp/tools/layout2/cpp/ReaderWriterTiledMapServiceOverview.h>

namespace csp
{
namespace layout
{

osgDB::ReaderWriter::ReadResult ReaderWriterTiledMapServiceOverview::readNode(const std::string& file, const osgDB::ReaderWriter::Options* opt) const
{
	std::string ext = osgDB::getLowerCaseFileExtension( file );
	if ( !acceptsExtension(ext) ) return ReadResult::FILE_NOT_HANDLED;

	std::istringstream fileNameParser( osgDB::getNameLessExtension(file) );
	unsigned int level, tileColumn, tileRow;
	fileNameParser >> level >> tileColumn >> tileRow;

	if ( !fileNameParser ) return ReadResult::FILE_NOT_HANDLED;

	osg::ref_ptr<osg::Node> node = buildNode(level, tileColumn, tileRow);
	if ( !node ) return ReadResult::ERROR_IN_READING_FILE;

	return node.release();
}

osg::ref_ptr<osg::Node> ReaderWriterTiledMapServiceOverview::buildNode(unsigned int level, unsigned int tileColumn, unsigned int tileRow) const
{
	int quartTileSize = OsgTerrainOverviewManipulator::TileSize / 4;

	osg::ref_ptr<osg::Node> topLeft     = placeNode( buildSubNode( level + 1, tileColumn * 2,     tileRow * 2     ), -quartTileSize,  quartTileSize );
	osg::ref_ptr<osg::Node> topRight    = placeNode( buildSubNode( level + 1, tileColumn * 2 + 1, tileRow * 2     ),  quartTileSize,  quartTileSize );
	osg::ref_ptr<osg::Node> bottomLeft  = placeNode( buildSubNode( level + 1, tileColumn * 2,     tileRow * 2 + 1 ), -quartTileSize, -quartTileSize );
	osg::ref_ptr<osg::Node> bottomRight = placeNode( buildSubNode( level + 1, tileColumn * 2 + 1, tileRow * 2 + 1 ),  quartTileSize, -quartTileSize );

	osg::ref_ptr<osg::Group> root = new osg::Group();

	root->addChild( topLeft.get() );
	root->addChild( topRight.get() );
	root->addChild( bottomLeft.get() );
	root->addChild( bottomRight.get() );

	return root.release();
}

osg::ref_ptr<osg::Node> ReaderWriterTiledMapServiceOverview::placeNode(osg::ref_ptr<osg::Node> node, double x, double y) const
{
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	pat->setPosition( osg::Vec3d( x, y, 0 ) );
	pat->setScale( osg::Vec3d( 0.5, 0.5, 0.5 ) );
	pat->addChild( node.get() );
	return pat.release();
}

osg::ref_ptr<osg::Node> ReaderWriterTiledMapServiceOverview::buildSubNode(unsigned int level, unsigned int tileColumn, unsigned int tileRow) const
{
	bool inherited;
	osg::ref_ptr<osg::Node> tile = buildTile(level, tileColumn, tileRow, inherited);
	if ( !tile ) return new osg::Group();

	if ( inherited || level >= getMaxLevel() ) return tile;

	std::ostringstream subTileFileName;
	subTileFileName << level << " " << tileColumn << " " << tileRow << "." << getExtension();

	osg::ref_ptr<osg::PagedLOD> pagedLOD = new osg::PagedLOD();

	pagedLOD->addChild( tile.get() );
	pagedLOD->setRange( 0, OsgTerrainOverviewManipulator::LodDistance, FLT_MAX );

	pagedLOD->setFileName( 1, subTileFileName.str() );
	pagedLOD->setRange( 1, 0, OsgTerrainOverviewManipulator::LodDistance );

	pagedLOD->setCenter( pagedLOD->getBound().center() );

	return pagedLOD.release();
}

osg::ref_ptr<osg::Node> ReaderWriterTiledMapServiceOverview::buildTile(unsigned int level, unsigned int tileColumn, unsigned int tileRow, bool & inherited) const
{
	InheritedImage inheritedImage = getImageOrInherit(level, tileColumn, tileRow);
	if ( !inheritedImage.image ) return 0;

	inherited = inheritedImage.inherited;

	osg::ref_ptr<osg::Group> tile = new osg::Group();

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	tile->addChild( geode.get() );

	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	geode->addDrawable( geometry.get() );

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	geometry->setVertexArray( vertices.get() );

	int halfTileSize = OsgTerrainOverviewManipulator::TileSize / 2;
	vertices->reserve(4);
	vertices->push_back( osg::Vec3( -halfTileSize, -halfTileSize, 0.0f ) ); // bottomLeft
	vertices->push_back( osg::Vec3(  halfTileSize, -halfTileSize, 0.0f ) ); // bottomRight
	vertices->push_back( osg::Vec3(  halfTileSize,  halfTileSize, 0.0f ) ); // topRight
	vertices->push_back( osg::Vec3( -halfTileSize,  halfTileSize, 0.0f ) ); // topLeft

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	geometry->setColorArray( colors.get() );
	geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
	osg::Vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
	colors->push_back( white );

	osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
	geometry->setTexCoordArray( 0, texcoords.get() );

	texcoords->reserve(4);
	texcoords->push_back( inheritedImage.texcoords.bottomLeft );
	texcoords->push_back( inheritedImage.texcoords.bottomRight );
	texcoords->push_back( inheritedImage.texcoords.topRight );
	texcoords->push_back( inheritedImage.texcoords.topLeft );

	geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, 4 ) );

	osg::StateSet* stateset = geometry->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	stateset->setTextureAttributeAndModes( 0, texture.get() );
	texture->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
	texture->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );
	//texture->setUseHardwareMipMapGeneration( false ); // On my ATI 1950 Pro, this solve a strange mess with mipmap

	texture->setImage( inheritedImage.image.get() );

	return tile.release();
}

ReaderWriterTiledMapServiceOverview::InheritedImage ReaderWriterTiledMapServiceOverview::getImageOrInherit(unsigned int level, unsigned int tileColumn, unsigned int tileRow) const
{
	InheritedImage inheritedImage;

	while ( true )
	{
		inheritedImage.image = getImage(level, tileColumn, tileRow);

		if ( inheritedImage.image.valid() || level == 0 ) break;

		// If we can't get the image at requested level, then
		// we try to use the quart of the lower level image
		inheritedImage.inherited = true;

		bool left = (tileColumn & 1) == 0;
		bool top = (tileRow & 1) == 0;

		level -= 1;
		tileColumn /= 2;
		tileRow /= 2;

		if ( left )
			inheritedImage.texcoords.shrinkToLeft();
		else
			inheritedImage.texcoords.shrinkToRight();

		if ( top )
			inheritedImage.texcoords.shrinkToTop();
		else
			inheritedImage.texcoords.shrinkToBottom();
	}

	return inheritedImage;
}

osg::ref_ptr<osg::Image> ReaderWriterTiledMapServiceOverview::getImage(unsigned int level, unsigned int tileColumn, unsigned int tileRow) const
{
	std::string fileName;
	std::string url;
	getFileNameAndUrl(level, tileColumn, tileRow, fileName, url);

	std::string cachedFileName;
	const char* cacheDir = getenv( "LAYOUT_CACHE_DIR" );
	if ( cacheDir )
	{
		std::ostringstream path;
		path << cacheDir << "/" << fileName;
		cachedFileName = path.str();
	}

	if ( !cachedFileName.empty() )
	{
		std::string fileFullName = osgDB::findDataFile( cachedFileName );
		if ( !fileFullName.empty() )
		{
			osg::ref_ptr<osg::Image> image = osgDB::readImageFile( cachedFileName );
			if ( image.valid() )
			{
				osg::notify(osg::INFO) << "Read cache " << cachedFileName << std::endl;
				return image;
			}
		}
	}

	osg::ref_ptr<osg::Image> image = osgDB::readImageFile( url );
	if ( !image )
	{
		osg::notify(osg::NOTICE) << "Error: can't read image " << url << std::endl;
	}
	else
	{
		osg::notify(osg::INFO) << "Read image " << url << std::endl;

		if ( !cachedFileName.empty() )
		{
			if ( !osgDB::writeImageFile( *image.get(), cachedFileName ) )
			{
				osg::notify(osg::NOTICE) << "Error: can't write cache " << cachedFileName << std::endl;
			}
			else
			{
				osg::notify(osg::INFO) << "Write cache " << cachedFileName << std::endl;
			}
		}
	}

	return image;
}

ReaderWriterTiledMapServiceOverview::InheritedImage::InheritedImage()
	: inherited( false )
{
}

ReaderWriterTiledMapServiceOverview::InheritedImage::TexCoord::TexCoord()
	: bottomLeft( 0.0f, 0.0f )
	, bottomRight( 1.0f, 0.0f )
	, topRight( 1.0f, 1.0f )
	, topLeft( 0.0f, 1.0f )
{
}

void ReaderWriterTiledMapServiceOverview::InheritedImage::TexCoord::shrinkToLeft()
{
	osg::Vec2::value_type middle = ( topLeft.x() + topRight.x() ) / 2;
	bottomRight.x() = middle;
	topRight.x() = middle;
}

void ReaderWriterTiledMapServiceOverview::InheritedImage::TexCoord::shrinkToRight()
{
	osg::Vec2::value_type middle = ( topLeft.x() + topRight.x() ) / 2;
	bottomLeft.x() = middle;
	topLeft.x() = middle;
}

void ReaderWriterTiledMapServiceOverview::InheritedImage::TexCoord::shrinkToTop()
{
	osg::Vec2::value_type middle = ( topLeft.y() + bottomLeft.y() ) / 2;
	bottomLeft.y() = middle;
	bottomRight.y() = middle;
}

void ReaderWriterTiledMapServiceOverview::InheritedImage::TexCoord::shrinkToBottom()
{
	osg::Vec2::value_type middle = ( topLeft.y() + bottomLeft.y() ) / 2;
	topRight.y() = middle;
	topLeft.y() = middle;
}

} // namespace layout
} // namespace csp
