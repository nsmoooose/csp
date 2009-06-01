#ifndef __CSPWF_WINDOWMANAGERVIEWERNODE_H__
#define __CSPWF_WINDOWMANAGERVIEWERNODE_H__

#include <csp/csplib/util/Ref.h>
#include <csp/cspwf/WindowManager.h>
#include <csp/cspwf/WindowManagerViewer.h>
#include <osg/Group>

namespace csp {
namespace wf {

/** Osg node class to insert the entire window manager into the tree.
 * This is usable when you want to create the window manager and attach
 * it to the scene graph.
 * This class will also create the WindowManagerViewer instance for you
 * with the width and height specified in the constructor.
 */
class CSPWF_EXPORT WindowManagerViewerNode : public osg::Group {
public:
		WindowManagerViewerNode(int width, int height);

		/** Returns the window manager.
		 */
		virtual Ref<WindowManagerViewer> getWindowManager();

private:
		Ref<WindowManagerViewer> m_WindowManager;
};

} // namespace wf
} // namespace csp


#endif // __CSPWF_WINDOWMANAGERVIEWERNODE_H__
