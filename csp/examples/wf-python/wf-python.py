#!/usr/bin/python
import csp.cspsim
import os.path
import sys
from csp.data.ui.scripts.utils import SlotManager

class HelloWorldWindow(csp.cspsim.Window, SlotManager):
    """A simple window script in python. It simply:
    Loads the layout of the window from a xml file.
    Finds a button using the id of the button.
    If the button was found we attach a click handler to it."""
    def __init__(self):
        csp.cspsim.Window.__init__(self)
        SlotManager.__init__(self)

        serializer = csp.cspsim.Serialization()
        serializer.load(self, 'hello_world.xml')
        
        helloButton = self.getById('hello')
        if helloButton != None:
            self.connectToClickSignal(helloButton, self.hello_Click)

    def hello_Click(self):
        """Do something in response to the click of the button."""
        print("Hello from python")

try:
    # We need a viewer that is capable of rendering.
    viewer = csp.cspsim.CSPViewer()
    # Add some osg handlers. (provides frame statistics and help)
    viewer.addStatsHandler();
    viewer.addHelpHandler();

    # We must have a window manager. The window manager is responsible
    # for all windows displayed.
    windowManager = csp.cspsim.WindowManagerViewer()
    viewer.addChild(windowManager.getRootNode())

    # We must also tell the windowing framework how to locate resources.
    # A resource may be a xml file or image files.
    # This is fixed by setting the default resource locator class.
    resourceLocator = csp.cspsim.ResourceLocator()
    resourceLocator.addFolder(os.path.dirname(sys.argv[0]))
    csp.cspsim.setDefaultResourceLocator(resourceLocator)

    # Lets create our hello world class. and add it to the window
    # manager.
    window = HelloWorldWindow()
    windowManager.show(window)

    # We have setup a viewer. We have added a window to the scene. So
    # lets run the rendering loop.
    viewer.run()
except csp.cspsim.SerializationException, message:
    print(message.what())

