#include <osgDB/ReadFile>
#include <csp/tools/layout2/cpp/OsgTerrainOverviewManipulator.h>
#include <csp/tools/layout2/cpp/OsgTerrainOverviewWindow.h>

namespace csp
{
namespace layout
{

OsgTerrainOverviewWindow::OsgTerrainOverviewWindow()
	: OsgGraphicsWindow( Ortho, new OsgTerrainOverviewManipulator )
{
}

void OsgTerrainOverviewWindow::loadModel(const std::string& file)
{
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile( file.c_str() );
	if ( node.valid() )
	{
		setSceneData( node.get() );
	}
}

} // namespace layout
} // namespace csp
