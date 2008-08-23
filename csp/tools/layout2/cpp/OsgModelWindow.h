#ifndef __CSP_LAYOUT_OSGMODELWINDOW_H__
#define __CSP_LAYOUT_OSGMODELWINDOW_H__

#include <csp/tools/layout2/cpp/OsgGraphicsWindow.h>

namespace csp {
namespace layout {

	class OsgModelWindow : public OsgGraphicsWindow {
	public:
		void loadModel(const std::string& file);
	};

} // namespace layout
} // namespace csp


#endif // __CSP_LAYOUT_OSGMODELWINDOW_H__
