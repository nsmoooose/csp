#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspwf/Label.h>
#include <csp/cspwf/Window.h>
#include <csp/cspwf/WindowManagerViewer.h>

using namespace csp;
using namespace csp::wf;

void createModel(WindowManager* windowManager) {
	// Create a window. A window can only contain a single
	// control that will span the entire width and height of
	// the parent (the window). By adding an other container
	// class to the window you can force other layout types.
	Ref<Window> window = new Window();
	window->getStyle()->setBackgroundColor(osg::Vec4(0, 1, 0, 1));
	window->getStyle()->setBorderWidth(1.0);
	window->getStyle()->setBorderColor(osg::Vec4(0, 0, 0, 1));
	window->getStyle()->setWidth(Style::UnitValue(Style::Pixels, 600));
	window->getStyle()->setHeight(Style::UnitValue(Style::Pixels, 400));

	// For the simplicity of this example we add a single 
	// label control.
	Ref<Label> label = new Label();
	label->setText("Hello world");
	label->getStyle()->setFontFamily(std::string("prima_sans_bt.ttf"));
	label->getStyle()->setFontSize(15);
	label->getStyle()->setColor(osg::Vec4(1, 0, 0, 1));

	// Add the label to the window control.
	window->setControl(label.get());

	windowManager->show(window.get());
}

int main(int, char**) {
	std::string search_path = "./data/fonts/";
	csp::ObjectModel::setDataFilePathList(search_path);

	Ref<WindowManagerViewer> windowManager = new WindowManagerViewer();

    // construct the viewer
    osgViewer::Viewer viewer;
    viewer.addEventHandler(new osgViewer::StatsHandler());
	viewer.addEventHandler(new osgViewer::HelpHandler());
    
    // Create a x, y, z model.
	createModel(windowManager.get());
    viewer.setSceneData(windowManager->getRootNode().get());

    // run the viewers frame loop
    return viewer.run();
}
