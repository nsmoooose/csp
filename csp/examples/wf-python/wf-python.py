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

        print("Loading hello_world.xml user interface file.")
        serializer = csp.cspsim.Serialization()
        serializer.load(self, 'hello_world.xml')
        
        helloButton = self.getById('hello')
        if helloButton != None:
            print("Connecting click event to button")
            self.connectToClickSignal(helloButton, self.hello_Click)

    def hello_Click(self):
        """Do something in response to the click of the button."""
        print("Hello from python")

try:
    # We need a viewer that is capable of rendering.
    print("Constructing the viewer")
    viewer = csp.cspsim.CSPViewer(0, 0, 640, 480)
    # Add some osg handlers. (provides frame statistics and help)
    viewer.addStatsHandler();
    viewer.addHelpHandler();

    # We must have a window manager. The window manager is responsible
    # for all windows displayed.
    print("Attaching a window manager to the scene graph.")
    windowManager = csp.cspsim.WindowManagerViewer()
    viewer.addChild(windowManager.getRootNode())

    # We are nothing without some mouse input. Lets get that as well.
    # We attach a osgGA event handler to the viewer. This event handler
    # will capture mouse and screen resize events and send them on
    # to the window framework.
    print("Adding a event handler so we can get mouse input.")
    viewer.addWindowManagerEventHandler(windowManager)

    # We must also tell the windowing framework how to locate resources.
    # A resource may be a xml file or image files.
    # This is fixed by setting the default resource locator class.
    resourceLocator = csp.cspsim.ResourceLocator()
    resourceLocator.addFolder(os.path.dirname(sys.argv[0]))
    resourceLocator.addFolder(os.path.dirname(sys.argv[0]) + "/../../data/fonts")
    print("Setting resource paths to: %s" % resourceLocator.getPathVariable())
    csp.cspsim.setDefaultResourceLocator(resourceLocator)

    # Lets create our hello world class. and add it to the window
    # manager.
    print("Creating the hello world window.")
    window = HelloWorldWindow()

    print("Attach event handler to window to support window movement.")
    windowMoveEventHandler = csp.cspsim.ControlMoveEventHandler(window)

    print("Showing the window")
    windowManager.show(window)

    # We have setup a viewer. We have added a window to the scene. So
    # lets run the rendering loop.
    print("Run the rendering loop.")
    viewer.run()
except csp.cspsim.SerializationException, message:
    print("An exception has occured.")
    print(message.what())
