#include <osgDB/ReadFile>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspwf/Label.h>
#include <csp/cspwf/MultiControlContainer.h>
#include <csp/cspwf/ResourceLocator.h>
#include <csp/cspwf/ScrollBar.h>
#include <csp/cspwf/Window.h>
#include <csp/cspwf/WindowManagerViewer.h>
#include <csp/cspwf/WindowManagerEventHandler.cpp>
#include <iostream>

using namespace csp;
using namespace csp::wf;

void addStyleSheetToWindow(Window* window) {
	// This is the css class of the window.
	Ref<Style> windowStyle = new Style();
	windowStyle->setBackgroundColor(osg::Vec4(1, 1, 1, 10));
	windowStyle->setBorderWidth(1.0);
	windowStyle->setBorderColor(osg::Vec4(0, 0, 0, 1));
	windowStyle->setWidth(Style::UnitValue(Style::Pixels, 600));
	windowStyle->setHeight(Style::UnitValue(Style::Pixels, 400));
	windowStyle->setHorizontalAlign(std::string("center"));
	windowStyle->setVerticalAlign(std::string("middle"));
	window->addNamedStyle("Window", windowStyle.get());

	// This is the css class of a vertical scrollbar.
	// Independent on how many we create we know that they will
	// have the same width and background color.
	Ref<Style> verticalStyle = new Style();
	verticalStyle->setWidth(Style::UnitValue(Style::Pixels, 20));
	verticalStyle->setBackgroundColor(osg::Vec4(0.8, 0.8, 0.8, 1.0));
	window->addNamedStyle("VerticalScrollBar", verticalStyle.get());

	// This is the css class of a horizontal scrollbar.
	// Independent on how many we create we know that they will
	// have the same width and background color.
	Ref<Style> horizontalStyle = new Style();
	horizontalStyle->setHeight(Style::UnitValue(Style::Pixels, 20));
	horizontalStyle->setBackgroundColor(osg::Vec4(0.8, 0.8, 0.8, 1.0));
	window->addNamedStyle("HorizontalScrollBar", horizontalStyle.get());

	// The scroll left button is always 20 pixels rectangle placed
	// to the left in the parent scrollbar control.
	Ref<Style> scrollLeftStyle = new Style();
	scrollLeftStyle->setHeight(Style::UnitValue(Style::Pixels, 20));
	scrollLeftStyle->setWidth(Style::UnitValue(Style::Pixels, 20));
	scrollLeftStyle->setLeft(Style::UnitValue(Style::Pixels, 0));
	scrollLeftStyle->setTop(Style::UnitValue(Style::Pixels, 0));
	scrollLeftStyle->setBackgroundColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
	window->addNamedStyle("ScrollLeftButton", scrollLeftStyle.get());

	Ref<Style> scrollRightStyle = new Style();
	scrollRightStyle->setHeight(Style::UnitValue(Style::Pixels, 20));
	scrollRightStyle->setWidth(Style::UnitValue(Style::Pixels, 20));
	scrollRightStyle->setHorizontalAlign(std::string("right"));
	scrollRightStyle->setTop(Style::UnitValue(Style::Pixels, 0));
	scrollRightStyle->setBackgroundColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
	window->addNamedStyle("ScrollRightButton", scrollRightStyle.get());

	Ref<Style> scrollUpStyle = new Style();
	scrollUpStyle->setHeight(Style::UnitValue(Style::Pixels, 20));
	scrollUpStyle->setWidth(Style::UnitValue(Style::Pixels, 20));
	scrollUpStyle->setLeft(Style::UnitValue(Style::Pixels, 0));
	scrollUpStyle->setTop(Style::UnitValue(Style::Pixels, 0));
	scrollUpStyle->setBackgroundColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
	window->addNamedStyle("ScrollUpButton", scrollUpStyle.get());

	Ref<Style> scrollDownStyle = new Style();
	scrollDownStyle->setHeight(Style::UnitValue(Style::Pixels, 20));
	scrollDownStyle->setWidth(Style::UnitValue(Style::Pixels, 20));
	scrollDownStyle->setLeft(Style::UnitValue(Style::Pixels, 0));
	scrollDownStyle->setVerticalAlign(std::string("bottom"));
	scrollDownStyle->setBackgroundColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
	window->addNamedStyle("ScrollDownButton", scrollDownStyle.get());
}

Ref<Window> createWindow() {
	// Create a window. A window can only contain a single
	// control that will span the entire width and height of
	// the parent (the window). By adding an other container
	// class to the window you can force other layout types.
	Ref<Window> window = new Window();

	// Add a special container that allow us to have more than
	// one child control. It also allow us to position each control
	// with absolute coordinates.
	Ref<MultiControlContainer> container = new MultiControlContainer();
	window->setControl(container.get());

	Ref<VerticalScrollBar> vertical = new VerticalScrollBar();
	vertical->getStyle()->setHeight(Style::UnitValue(Style::Pixels, 378));
	vertical->getStyle()->setTop(Style::UnitValue(Style::Pixels, 0));
	vertical->getStyle()->setHorizontalAlign(std::string("right"));
	container->addControl(vertical.get());

	Ref<HorizontalScrollBar> horizontal = new HorizontalScrollBar();
	horizontal->getStyle()->setWidth(Style::UnitValue(Style::Pixels, 578));
	horizontal->getStyle()->setLeft(Style::UnitValue(Style::Pixels, 0));
	horizontal->getStyle()->setVerticalAlign(std::string("bottom"));
	container->addControl(horizontal.get());

	// For the simplicity of this example we add a single 
	// label control to the container. Since we have added a width
	// and a height it is now possible to use alignment.
	Ref<Label> label = new Label();
	label->setId("label");
	label->setText("Here you have two scrollbars. One vertical and one horizontal.");
	label->getStyle()->setWidth(Style::UnitValue(Style::Pixels, 300));
	label->getStyle()->setHeight(Style::UnitValue(Style::Pixels, 30));
	label->getStyle()->setFontFamily(std::string("prima_sans_bt.ttf"));
	label->getStyle()->setFontSize(15);
	label->getStyle()->setColor(osg::Vec4(0, 0, 0, 1));
	label->getStyle()->setHorizontalAlign(std::string("center"));
	label->getStyle()->setVerticalAlign(std::string("middle"));

	// Add the label to the window control.
	container->addControl(label.get());

	return window;
}

int main(int, char**) {
	log().setCategories(cLogCategory_ALL);
	log().setPriority(cLogPriority_DEBUG);

	// Make sure that the windowing framework can external files.
	// For example fonts.
	Ref<csp::wf::ResourceLocator> resourceLocator = new csp::wf::ResourceLocator();
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
	addStyleSheetToWindow(window.get());
	windowManager->show(window.get());

    // run the viewers frame loop
    return viewer.run();
}
