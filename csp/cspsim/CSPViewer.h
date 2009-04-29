#ifndef __CSPSIM_CSPVIEWER_H__
#define __CSPSIM_CSPVIEWER_H__

#include <csp/cspsim/Export.h>

namespace osg {
	class Node;
}

namespace osgViewer {
	class Viewer;
}

namespace csp {
	namespace wf {
		class WindowManagerViewer;
	}

	/* Base class that encapsulates the osgViewer::Viewer class. This
	   class will run the main loop and is responsible for holding the
	   entire scene and render it. 

	   As default it will create a osg::Group and add set it on the viewer.
	   So it is only necessarry to add new children to it in order to display 
	   data.

	   It is also exported to python meaning it can be used from python
	   to start the simulator or do other things.*/
	class CSPSIM_EXPORT CSPViewer {
	public:
		CSPViewer();
		virtual ~CSPViewer();
		
		virtual void run();

		virtual void addChild(osg::Node* node);

		/* Provides statistics information if you press the s key. */
		virtual void addStatsHandler();
		/* Provides help if you press the h key. */
		virtual void addHelpHandler();

		virtual void addWindowManagerEventHandler(wf::WindowManagerViewer* windowManager);

	private:
		osg::ref_ptr<osgViewer::Viewer> m_Viewer;
	}; 
} // namespace csp

#endif /* __CSPSIM_CSPVIEWER_H__ */
