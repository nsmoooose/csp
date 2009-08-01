#ifndef __CSP_LAYOUT_OSGTERRAINWINDOW_H__
#define __CSP_LAYOUT_OSGTERRAINWINDOW_H__

#include <csp/tools/layout2/cpp/OsgGraphicsWindow.h>

namespace csp
{
namespace layout
{

class OsgTerrainOverviewWindow : public OsgGraphicsWindow
{
	public:
		OsgTerrainOverviewWindow();

		void loadModel(const std::string& file);

		void zoomOut(double distance);
		void zoomIn(double distance);
		void panLeft(double distance);
		void panRight(double distance);
		void panUp(double distance);
		void panDown(double distance);
};

} // namespace layout
} // namespace csp


#endif // __CSP_LAYOUT_OSGTERRAINWINDOW_H__
