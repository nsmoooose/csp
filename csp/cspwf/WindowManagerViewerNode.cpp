#include "WindowManagerViewerNode.h"

namespace csp {
namespace wf {

WindowManagerViewerNode::WindowManagerViewerNode(int width, int height) : 
	m_WindowManager(new WindowManagerViewer(width, height)) {
	
	addChild(m_WindowManager->getRootNode().get());
}

Ref<WindowManagerViewer> WindowManagerViewerNode::getWindowManager() {
	return m_WindowManager;
}

} // namespace wf
} // namespace csp
