#include <osgDB/ReadFile>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspwf/Label.h>
#include <csp/cspwf/MultiControlContainer.h>
#include <csp/cspwf/ResourceLocator.h>
#include <csp/cspwf/Window.h>
#include <csp/cspwf/WindowManagerViewer.h>
#include <csp/cspwf/WindowManagerEventHandler.cpp>
#include <iostream>

using namespace csp;
using namespace csp::wf;

void handleClick(ClickEventArgs& event) {
	std::cout << "Click on control with name: " << event.control->getName().c_str() <<
		" with coordinate x: " << event.x <<
		" and y: " << event.y << std::endl;
}

void handleHover(HoverEventArgs& event) {
	std::cout << "Hover on control with name: " << // event.control->getName().c_str() <<
		" with coordinate x: " << event.x <<
		" and y: " << event.y << std::endl;
}

void handleMouseDown(MouseButtonEventArgs& event) {
	std::cout << "Mouse button " << event.button <<
		" down on control with name: " << event.control->getName().c_str() <<
		" with coordinate x: " << event.x <<
		" and y: " << event.y << std::endl;
}

void handleMouseMove(MouseEventArgs& event) {
	std::cout << "Mouse move on control with name: " << event.control->getName().c_str() <<
		" with coordinate x: " << event.x <<
		" and y: " << event.y << std::endl;
}

void handleMouseUp(MouseButtonEventArgs& event) {
	std::cout << "Mouse button " << event.button <<
		" up on control with name: " << event.control->getName().c_str() <<
		" with coordinate x: " << event.x <<
		" and y: " << event.y << std::endl;
}

osg::ref_ptr<osg::Node> createWindow(WindowManagerViewer* windowManager) {
	// Create a window. A window can only contain a single
	// control that will span the entire width and height of
	// the parent (the window). By adding an other container
	// class to the window you can force other layout types.
	Ref<Window> window = new Window();
	window->getStyle()->setBackgroundColor(osg::Vec4(1, 1, 1, 0.8));
	window->getStyle()->setBorderWidth(1.0);
	window->getStyle()->setBorderColor(osg::Vec4(0, 0, 0, 1));
	window->getStyle()->setWidth(Style::UnitValue(Style::Pixels, 600));
	window->getStyle()->setHeight(Style::UnitValue(Style::Pixels, 400));
	window->getStyle()->setHorizontalAlign(std::string("center"));
	window->getStyle()->setVerticalAlign(std::string("middle"));
	window->Click.connect(sigc::ptr_fun(handleClick));
	window->Hover.connect(sigc::ptr_fun(handleHover));
	window->MouseDown.connect(sigc::ptr_fun(handleMouseDown));
	window->MouseMove.connect(sigc::ptr_fun(handleMouseMove));
	window->MouseUp.connect(sigc::ptr_fun(handleMouseUp));

	// Add a special container that allow us to have more than
	// one child control. It also allow us to position each control
	// with absolute coordinates.
	Ref<MultiControlContainer> container = new MultiControlContainer();
	window->setControl(container.get());

	// For the simplicity of this example we add a single 
	// label control to the container. Since we have added a width
	// and a height it is now possible to use alignment.
	Ref<Label> label = new Label();
	label->setText("Hello world");
	label->getStyle()->setWidth(Style::UnitValue(Style::Pixels, 100));
	label->getStyle()->setHeight(Style::UnitValue(Style::Pixels, 30));
	label->getStyle()->setFontFamily(std::string("prima_sans_bt.ttf"));
	label->getStyle()->setFontSize(15);
	label->getStyle()->setColor(osg::Vec4(1, 0, 0, 1));
	label->getStyle()->setHorizontalAlign(std::string("center"));
	label->getStyle()->setVerticalAlign(std::string("middle"));

	// Add the label to the window control.
	container->addControl(label.get());

	windowManager->show(window.get());
	return windowManager->getRootNode().get();
}

int main(int, char**) {
	// Make sure that the windowing framework can external files.
	// For example fonts.
	Ref<csp::wf::ResourceLocator> resourceLocator = new csp::wf::ResourceLocator();
	resourceLocator->addFolder("data/fonts");
	csp::wf::setDefaultResourceLocator(resourceLocator.get());

	// This instance keeps track of all windows connected to it.
	// It is used to open and close windows.
	Ref<WindowManagerViewer> windowManager = new WindowManagerViewer(640, 480);

    // construct the viewer
    osgViewer::Viewer viewer;
	viewer.setUpViewInWindow(40, 40, 640, 480);
    viewer.addEventHandler(new osgViewer::StatsHandler());
	viewer.addEventHandler(new osgViewer::HelpHandler());
	viewer.addEventHandler(new WindowManagerEventHandler(windowManager.get()));
    
	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(createWindow(windowManager.get()).get());
    viewer.setSceneData(group.get());

    // run the viewers frame loop
    return viewer.run();
}
