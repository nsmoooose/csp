#!/usr/bin/python
import os.path
import sys
import csp.csplib
import csp.cspsim
from csp.data.ui.scripts.windows.desktop import Desktop
from csp.data.ui.scripts.windows.mainmenu import MainMenu
from csp.data.ui.scripts.windows.topmenu import TopMenu

try:
    # We need a viewer that is capable of rendering.
    viewer = csp.cspsim.CSPViewer()
    # Add some osg handlers. (provides frame statistics and help)
    viewer.addStatsHandler()
    viewer.addHelpHandler()

    # We must have a window manager. The window manager is responsible
    # for all windows displayed.
    windowManager = csp.cspsim.WindowManagerViewer()
    viewer.addChild(windowManager.getRootNode())
    viewer.addWindowManagerEventHandler(windowManager)

    # We must also tell the windowing framework how to locate resources.
    # A resource may be a xml file or image files.
    # This is fixed by setting the default resource locator class.
    resourceLocator = csp.cspsim.ResourceLocator()
    resourceLocator.addFolder("../data/fonts")
    resourceLocator.addFolder("../data")
    resourceLocator.addFolder("../data/ui")
    resourceLocator.addFolder("../data/ui/themes/default")
    resourceLocator.addFolder("../data/ui/localization/english")
    csp.cspsim.setDefaultResourceLocator(resourceLocator)

    # Create the background window that covers the entire screen.
    window = Desktop()
    windowManager.show(window)
    window.maximizeWindow()

    # We do want a menu at the top of the screen.
    window = TopMenu(None)
    windowManager.show(window)

    # And we want the main menu.
    window = MainMenu(None)
    windowManager.show(window)

    # We have setup a viewer. We have added windows to the scene. So
    # lets run the rendering loop.
    viewer.run()
except csp.cspsim.SerializationException, message:
    print(message.what())
