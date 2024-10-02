#pragma once

#include <osg/Group>

namespace csp {
	/* This class is responsible for the screen info node.
	   This node is displaying frame rate and several other
	   important textual information to the user.

	   It also provides convenient functions for getting, 
	   installing and uninstalling the info node. */
	class ScreenInfoNode : public osg::Group {
	public:
		/* Sets up the screen info node into the scene. */
		static void setUp(osg::Group* scene);

		/* Returns the group that screen information is 
		   displayed into. Returns null if not installed. */
		static osg::Group* getGroup(osg::Group* scene);

		/* Removes the screen info node from the scene. */
		static void tearDown(osg::Group* scene);
	};
}
