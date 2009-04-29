#ifndef __CSPWF_WINDOWMANAGERVIEWERNODE_H__
#define __CSPWF_WINDOWMANAGERVIEWERNODE_H__

#include <csp/csplib/util/Ref.h>
#include <csp/cspwf/WindowManager.h>
#include <csp/cspwf/WindowManagerViewer.h>
#include <osg/Group>

namespace csp {
namespace wf {

class CSPWF_EXPORT WindowManagerViewerNode : public osg::Group {
public:
		WindowManagerViewerNode(int width, int height);
		virtual Ref<WindowManagerViewer> getWindowManager();

private:
		Ref<WindowManagerViewer> m_WindowManager;
};

} // namespace wf
} // namespace csp


#endif // __CSPWF_WINDOWMANAGERVIEWERNODE_H__
