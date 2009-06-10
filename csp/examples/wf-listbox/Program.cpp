#include <osgDB/ReadFile>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspwf/Label.h>
#include <csp/cspwf/ListBox.h>
#include <csp/cspwf/MultiControlContainer.h>
#include <csp/cspwf/ResourceLocator.h>
#include <csp/cspwf/ScrollBar.h>
#include <csp/cspwf/Window.h>
#include <csp/cspwf/WindowManagerViewer.h>
#include <csp/cspwf/WindowManagerEventHandler.cpp>
#include <iostream>
#include <strstream>

using namespace csp;
using namespace csp::wf;

Ref<Window> createWindow() {
	// Create a window. A window can only contain a single
	// control that will span the entire width and height of
	// the parent (the window). By adding an other container
	// class to the window you can force other layout types.
	Ref<Window> window = new Window();

    // This call will create some default style objects that
    // will provide us with basic visual appearence. Nothing
    // fancy. Just some borders, background color etc.
    window->createDefaultStyle();

    // We describe the window position and size with this
    // named css class.
    Ref<Style> windowStyle = new Style();
	windowStyle->setWidth(Style::UnitValue(Style::Pixels, 600));
	windowStyle->setHeight(Style::UnitValue(Style::Pixels, 400));
	windowStyle->setHorizontalAlign(std::string("center"));
	windowStyle->setVerticalAlign(std::string("middle"));
	window->addNamedStyle("my_window", windowStyle.get());
    window->setCssClass(std::string("my_window"));

	// Add a special container that allow us to have more than
	// one child control. It also allow us to position each control
	// with absolute coordinates.
	Ref<MultiControlContainer> container = new MultiControlContainer();
	window->setControl(container.get());

    // Create a standard listbox.
	Ref<ListBox> listbox = new ListBox();
    Ref<Style> listboxStyle = listbox->getStyle();
    listboxStyle->setLeft(Style::UnitValue(Style::Pixels, 10));
    listboxStyle->setTop(Style::UnitValue(Style::Pixels, 10));
	listboxStyle->setWidth(Style::UnitValue(Style::Pixels, 100));
	listboxStyle->setHeight(Style::UnitValue(Style::Pixels, 380));
	container->addControl(listbox.get());

    // Nothing fun without any items as well....
	for(int index=0;index < 14;++index) {
	    std::strstream text;
	    text << "Item no: " << index << std::ends;
        Ref<ListBoxItem> item = new ListBoxItem(text.str());
        listbox->addItem(item.get());

        if(index == 3) {
            listbox->setSelectedItem(item.get());
        }
	}

	Ref<Label> label = new Label();
	label->setId("label");
	label->setText("This is a window with a single list box.");
	label->getStyle()->setWidth(Style::UnitValue(Style::Pixels, 300));
	label->getStyle()->setHeight(Style::UnitValue(Style::Pixels, 30));
	label->getStyle()->setHorizontalAlign(std::string("center"));
	label->getStyle()->setVerticalAlign(std::string("middle"));
    container->addControl(label.get());

	return window;
}

int main(int, char**) {
	log().setCategories(cLogCategory_ALL);
	log().setPriority(cLogPriority_DEBUG);

	// Make sure that the windowing framework can external files.
	// For example fonts.
	Ref<csp::wf::ResourceLocator> resourceLocator = new csp::wf::ResourceLocator();
	resourceLocator->addFolder("data/ui/themes/default/images");
	resourceLocator->addFolder("data/fonts");
	csp::wf::setDefaultResourceLocator(resourceLocator.get());

	// This instance keeps track of all windows connected to it.
	// It is used to open and close windows.
	Ref<WindowManagerViewer> windowManager = new WindowManagerViewer(1024, 768);

    // construct the viewer
    osgViewer::Viewer viewer;
	viewer.setUpViewInWindow(40, 40, 1024, 768);
    viewer.addEventHandler(new osgViewer::StatsHandler());
	viewer.addEventHandler(new osgViewer::HelpHandler());
	viewer.addEventHandler(new WindowManagerEventHandler(windowManager.get()));

	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(windowManager->getRootNode().get());
    viewer.setSceneData(group.get());

	// Create the window and show it.
	Ref<Window> window = createWindow();
	windowManager->show(window.get());

    // run the viewers frame loop
    return viewer.run();
}
