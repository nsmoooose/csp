#include <osgGA/TrackballManipulator>
#include <csp/tools/layout2/cpp/OsgTrackballGraphicsWindow.h>

namespace csp
{
namespace layout
{

OsgTrackballGraphicsWindow::OsgTrackballGraphicsWindow()
	: OsgGraphicsWindow( Perspective, new osgGA::TrackballManipulator )
{
}

osg::ref_ptr<osgGA::TrackballManipulator> OsgTrackballGraphicsWindow::getTrackballManipulator()
{
	return dynamic_cast<osgGA::TrackballManipulator *>( getManipulator().get() );
}

void OsgTrackballGraphicsWindow::zoomOut(double distance)
{
	osg::ref_ptr<osgGA::TrackballManipulator> manipulator = getTrackballManipulator();
	manipulator->setDistance(manipulator->getDistance() + distance);
}

void OsgTrackballGraphicsWindow::zoomIn(double distance)
{
	osg::ref_ptr<osgGA::TrackballManipulator> manipulator = getTrackballManipulator();
	manipulator->setDistance(manipulator->getDistance() - distance);
}

void OsgTrackballGraphicsWindow::panLeft(double /*distance*/)
{
}

void OsgTrackballGraphicsWindow::panRight(double /*distance*/)
{
}

void OsgTrackballGraphicsWindow::panUp(double /*distance*/)
{
}

void OsgTrackballGraphicsWindow::panDown(double /*distance*/)
{
}

} // namespace layout
} // namespace csp
