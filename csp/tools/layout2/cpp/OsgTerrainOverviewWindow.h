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
};

} // namespace layout
} // namespace csp


#endif // __CSP_LAYOUT_OSGTERRAINWINDOW_H__
