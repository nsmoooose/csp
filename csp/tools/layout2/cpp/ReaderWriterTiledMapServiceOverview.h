#ifndef __CSP_LAYOUT_READERWRITERTILEDMAPSERVICEOVERVIEW_H__
#define __CSP_LAYOUT_READERWRITERTILEDMAPSERVICEOVERVIEW_H__

#include <osgDB/ReaderWriter>

namespace csp
{
namespace layout
{

class ReaderWriterTiledMapServiceOverview : public osgDB::ReaderWriter
{
	public:
		virtual const char* className() const { return "ReaderWriterTiledMapServiceOverview"; }

		virtual osgDB::ReaderWriter::ReadResult readNode(const std::string& file, const osgDB::ReaderWriter::Options* opt) const;

	protected:
		struct InheritedImage
		{
			InheritedImage();

			osg::ref_ptr<osg::Image> image;
			bool inherited;

			struct TexCoord
			{
				TexCoord();
				void shrinkToLeft();
				void shrinkToRight();
				void shrinkToTop();
				void shrinkToBottom();

				osg::Vec2 bottomLeft;
				osg::Vec2 bottomRight;
				osg::Vec2 topRight;
				osg::Vec2 topLeft;
			} texcoords;
		};

		osg::ref_ptr<osg::Node> buildNode(unsigned int level, unsigned int tileColumn, unsigned int tileRow) const;
		osg::ref_ptr<osg::Node> placeNode(osg::ref_ptr<osg::Node> node, double x, double y) const;
		osg::ref_ptr<osg::Node> buildSubNode(unsigned int level, unsigned int tileColumn, unsigned int tileRow) const;
		osg::ref_ptr<osg::Node> buildTile(unsigned int level, unsigned int tileColumn, unsigned int tileRow, bool & inherited) const;
		InheritedImage getImageOrInherit(unsigned int level, unsigned int tileColumn, unsigned int tileRow) const;
		osg::ref_ptr<osg::Image> getImage(unsigned int level, unsigned int tileColumn, unsigned int tileRow) const;

		virtual unsigned int getMaxLevel() const = 0;
		virtual std::string getExtension() const = 0;
		virtual void getFileNameAndUrl(unsigned int level, unsigned int tileColumn, unsigned int tileRow, std::string & fileName, std::string & url) const = 0;
};

} // namespace layout
} // namespace csp

#endif // __CSP_LAYOUT_READERWRITERTILEDMAPSERVICEOVERVIEW_H__
