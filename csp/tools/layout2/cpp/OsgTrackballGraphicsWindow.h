#ifndef __CSP_LAYOUT_OSGTRACKBALLGRAPHICSWINDOW_H__
#define __CSP_LAYOUT_OSGTRACKBALLGRAPHICSWINDOW_H__

#include <csp/tools/layout2/cpp/OsgGraphicsWindow.h>

namespace osgGA
{
	class TrackballManipulator;
}

namespace csp
{
namespace layout
{

class OsgTrackballGraphicsWindow : public OsgGraphicsWindow
{
	public:
		OsgTrackballGraphicsWindow();

		void zoomOut(double distance);
		void zoomIn(double distance);
		void panLeft(double distance);
		void panRight(double distance);
		void panUp(double distance);
		void panDown(double distance);

	protected:
		osg::ref_ptr<osgGA::TrackballManipulator> getTrackballManipulator();
};

} // namespace layout
} // namespace csp


#endif // __CSP_LAYOUT_OSGTRACKBALLGRAPHICSWINDOW_H__
