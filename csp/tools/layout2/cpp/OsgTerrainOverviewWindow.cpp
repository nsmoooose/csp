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

void OsgTerrainOverviewWindow::zoomOut(double /*distance*/)
{
	osg::ref_ptr<OsgTerrainOverviewManipulator> manipulator = dynamic_cast<OsgTerrainOverviewManipulator *>( getManipulator().get() );
	manipulator->zoomOut();
}

void OsgTerrainOverviewWindow::zoomIn(double /*distance*/)
{
	osg::ref_ptr<OsgTerrainOverviewManipulator> manipulator = dynamic_cast<OsgTerrainOverviewManipulator *>( getManipulator().get() );
	manipulator->zoomIn();
}

void OsgTerrainOverviewWindow::panLeft(double /*distance*/)
{
}

void OsgTerrainOverviewWindow::panRight(double /*distance*/)
{
}

void OsgTerrainOverviewWindow::panUp(double /*distance*/)
{
}

void OsgTerrainOverviewWindow::panDown(double /*distance*/)
{
}

} // namespace layout
} // namespace csp
