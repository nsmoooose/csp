#include <csp/csplib/data/Date.h>
#include <csp/csplib/data/DataArchive.h>
#include <csp/csplib/data/DataManager.h>
#include <csp/csplib/util/FileUtility.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/SimpleConfig.h>
#include <csp/csplib/util/Modules.h>
#include <csp/csplib/data/InterfaceProxy.h>
#include <csp/cspsim/Export.h>
#include <csp/cspsim/RegisterObjectInterfaces.h>
#include <csp/cspsim/Config.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/sky/Sky.h>
#include <csp/cspsim/Theater.h>
#include <csp/cspsim/TerrainObject.h>
#include <csp/cspsim/Shader.h>
#include <csp/cspsim/VirtualScene.h>
#include <csp/modules/demeter/DemeterTerrain.h>

#include <osg/Camera>
#include <osg/NodeVisitor>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgGA/GUIActionAdapter>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>

#include "SDLViewer.h"
#include "SkyGroup.h"

#include <csp/cspwf/Label.h>
#include <csp/cspwf/MultiControlContainer.h>
#include <csp/cspwf/ResourceLocator.h>
#include <csp/cspwf/Window.h>
#include <csp/cspwf/WindowManagerViewer.h>
#include <csp/cspwf/WindowManagerViewerNode.h>

#include <iostream>

using namespace csp;

void addSky(csp::SDLViewer& sdlViewer, osg::Group* parentNode);
void addUserInterface(csp::SDLViewer& sdlViewer, osg::Group* parentNode);
void addTerrain(csp::SDLViewer& sdlViewer, osg::Group* parentNode);

Ref<csp::DataManager> m_DataManager;
Ref<csp::Theater> m_Theater;
Ref<csp::TerrainObject> m_Terrain;
Ref<csp::VirtualScene> vScene;
osg::ref_ptr<osg::State> m_GlobalState;

csp::SDLViewer viewer;

class ModuleLoader;

void loadDefault()
{
	std::cout << "loading default scene...\n";
	// Do some global configuration to make things work.
	std::string search_path = "./data/images";
	csp::ObjectModel::setDataFilePathList(search_path);
	setDataPath("TerrainPath", "data/terrain");

	// The root node that will contain everything.
	osg::ref_ptr<osg::Group> root = new osg::Group();

//	csp::SDLViewer viewer;
	viewer.initialize();
	viewer.getViewer()->setSceneData(root.get());

	addSky(viewer, root.get());
	addTerrain(viewer, root.get());
	addUserInterface(viewer, root.get());
}

// loadTheater is derived from CSPSim::loadSimulation
void loadTheater(std::string theaterName)
{
	// CSPSim calls this function via sim.py, so it does not exist in CSPSim::loadSimulation
	registerAllObjectInterfaces();

	// We have to manually load the Demeter module to properly register its object interface
	// For dynamic modules like Demeter, this is not handled by registerAllObjectInterfaces()
	// A similar call is made in sim.py
	bool e = csp::ModuleLoader::load("modules\\demeter\\.bin\\demeter.dll");
	assert(e == true);
	
	// opening the config is also done in sim.py
	// important for Windows users: start the application from the CSP root directory,
	// NOT from the directory where sdk-viewer.exe is located. Otherwise the application
	// will not find the data paths
	bool isConfig = openConfig("examples\\sdl-viewer\\sdl-viewer.ini", 1);
	assert(isConfig == true);

	// Do some global configuration to make things work.
	std::string search_path = "./data/images/;./data/models;./data/Terrain";
	csp::ObjectModel::setDataFilePathList(search_path);
	csp::Shader::instance()->setShaderPath("./data/shaders");

	m_DataManager = new DataManager();

	// open the primary data archive
	std::string cache_path = getCachePath();
	std::string archive_file = ospath::join(cache_path, "bin\\sim.dar");
	try {
		DataArchive *sim = new DataArchive(archive_file.c_str(), 1);
		assert(sim);
		m_DataManager->addArchive(sim);
	}
	catch (Exception &e) {
		std::cout << "Error opening data archive " << archive_file;
		std::cout << e.getType() << ": " << e.getMessage();
		throw e;
		//::exit(0);
	}

	// It's important to create the viewer before the terrain engine is instantiated
	// Otherwise Demeter will not be able to properly locate the OpenGL drivers
	viewer.initialize();

	std::cout << "Initializing theater";
	std::string theater = g_Config.getPath("Testing", "Theater", theaterName, false);
	// This will load ALL objects of the theater in memory
	m_Theater = m_DataManager->getObject(theater.c_str());
	assert(m_Theater.valid());
	m_Terrain = m_Theater->getTerrain();
	assert(m_Terrain.valid());
	m_Terrain->setScreenSizeHint(800, 600);
	m_Terrain->activate();

	// The root node that will contain everything.
	osg::ref_ptr<osg::Group> root = new osg::Group();
	viewer.getViewer()->setSceneData(root.get());

//	addSky(viewer, root.get());
//	addTerrain(viewer, root.get());

	// 
	std::cout << "Initializing scene graph";
	vScene = new VirtualScene(m_GlobalState.get(), 800, 600);
	vScene->buildScene();

//	root->addChild(vScene->getVeryFarGroup());
//	root->addChild(vScene->getFarGroup());
	root->addChild(vScene->getNearGroup());
//	vScene->setTerrain(m_Terrain);
}

int main(int argc, char** argv) {
	
	std::string theaterName = "default";

	if(argc > 1) {
		std::string argument;
		for(int i=1; i<argc; i++) {
			argument = argv[i];
			if(argument == "-d") {
				std::cout << "Attach debugger and press ENTER to continue...\n";
				std::cin.get();
			}
			if(argument == "-h" || argument == "--help" || argument == "/?") {
				std::cout << "sdl-viewer [-d] [Theater_Name]\n";
				std::cout << "\n Arguments:\n\n";
				std::cout << " -d\t\t- pause at startup to allow attaching a debugger\n";
				std::cout << " Theater_Name\t- name of theater to be loaded\n";
				return -1;
			}
			else {
				if(argument[0] != '-') {
					std::cout << "Trying to load theater sim:theater." << argument << "\n";
					theaterName = "sim:theater." + argument;
				}
			}
		}
	}
	
	if(theaterName == "default")
		loadDefault();
	else
		loadTheater(theaterName);

	viewer.getViewer()->setCameraManipulator(new osgGA::TrackballManipulator);
	viewer.getViewer()->addEventHandler(new osgViewer::StatsHandler);
	viewer.getViewer()->addEventHandler(new osgViewer::HelpHandler());

	viewer.run();

	return 0;
}

/** This is the date and time for this application. */
double global_date = 0.0;

/** When we have significantly changed date and time we can set this variable to force
 * 	a complete update of the sky dome. */
bool force_update_of_sky_dome = false;

/** This keyboard event handler makes is possible for you to advance date and time and
	look at the result. */
class DateTimeEventHandler : public osgGA::GUIEventHandler
{
public:
	/** Handle keyboard event. */
    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&) {
    	switch(ea.getEventType())
	    {
        	case(osgGA::GUIEventAdapter::KEYDOWN):
    	    {
	            if (ea.getKey()==osgGA::GUIEventAdapter::KEY_Right ||
                	ea.getKey()==osgGA::GUIEventAdapter::KEY_KP_Right)
            	{
					global_date += (1.0 / 24) / 4;
					force_update_of_sky_dome = true;
            	    return true;
        	    }
	            else if (ea.getKey()==osgGA::GUIEventAdapter::KEY_Left ||
    	                 ea.getKey()==osgGA::GUIEventAdapter::KEY_KP_Left)
            	{
					global_date -= (1.0 / 24) / 4;
					force_update_of_sky_dome = true;
                	return true;
            	}
            	return false;
        	}

        	default:
    	        return false;
	    }
	}

	/** We also provide some help on this application. */
    virtual void getUsage(osg::ApplicationUsage& usage) const {
	    usage.addKeyboardMouseBinding("Left Arrow", "+ 15 minutes");
    	usage.addKeyboardMouseBinding("Right Array", "- 15 minutes");
	}
};

/** This class is responsible for updating stars, sun, moon and the sky dome
 *	for each frame. */
class SkyUpdateCallback : public osg::NodeCallback
{
	virtual void operator()(osg::Node* node, osg::NodeVisitor*)
	{
		SkyGroup* skyGroup = dynamic_cast<SkyGroup*>(node);
		if(skyGroup) {
			skyGroup->sky()->update(global_date, force_update_of_sky_dome);
			force_update_of_sky_dome = false;
		}
	}
};

void addSky(csp::SDLViewer& sdlViewer, osg::Group* parentNode) {
	// Set the default date and time for this application.
	global_date = SimDate(2008, 12, 31, 23, 59, 0).getJulianDate();

	// The sky group contains stars, sky dome, sun and moon.
	osg::ref_ptr<SkyGroup> skyGroup = new SkyGroup();
	skyGroup->setUpdateCallback(new SkyUpdateCallback());

	parentNode->addChild(skyGroup.get());

	sdlViewer.getViewer()->addEventHandler(new DateTimeEventHandler());
}

void addUserInterface(csp::SDLViewer& /* sdlViewer */, osg::Group* parentNode) {
	using namespace csp;
	using namespace csp::wf;
	
	Ref<csp::wf::ResourceLocator> resourceLocator = new csp::wf::ResourceLocator();
	resourceLocator->addFolder("data/fonts");
	csp::wf::setDefaultResourceLocator(resourceLocator.get());

	// The purpose of this node is to to simply attach the entire window manager to the scenegraph 
	osg::ref_ptr<csp::wf::WindowManagerViewerNode> windowManagerNode = new csp::wf::WindowManagerViewerNode(800, 600);
	parentNode->addChild(windowManagerNode.get());

	Ref<WindowManagerViewer> windowManager = windowManagerNode->getWindowManager();

	// Create a window. A window can only contain a single
	// control that will span the entire width and height of
	// the parent (the window). By adding an other container
	// class to the window you can force other layout types.
	Ref<Window> window = new Window();
	window->getStyle()->setBackgroundColor(osg::Vec4(1, 1, 1, 0.8));
	window->getStyle()->setBorderWidth(1.0);
	window->getStyle()->setBorderColor(osg::Vec4(0, 0, 0, 1));
	window->getStyle()->setWidth(Style::UnitValue(Style::Pixels, 600));
	window->getStyle()->setHeight(Style::UnitValue(Style::Pixels, 100));
	window->getStyle()->setLeft(Style::UnitValue(Style::Pixels, 0));
	window->getStyle()->setTop(Style::UnitValue(Style::Pixels, 0));

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
}

/* Purpose of this class is to create the terrain objects and attach everything to the
tree of nodes. */
class TerrainGroup : public osg::Group {
public:
	TerrainGroup() {
		// These parameters is normally serialized from an xml file.
		m_Terrain = new DemeterTerrain();
		m_Terrain->setName("Balkan Terrain");
		m_Terrain->setVersion(0);
		m_Terrain->setUseDynamicTextures(false);
		m_Terrain->setUseTextureCompression(true);
		m_Terrain->setPreloadTextures(true);
		m_Terrain->setUseTextureFactory(false);
		m_Terrain->setMaxTriangles(60000);
		m_Terrain->setLattice(true);
		m_Terrain->setLatticeWidth(16);
		m_Terrain->setLatticeHeight(16);
		m_Terrain->setLatticeTilesWidth(2);
		m_Terrain->setLatticeTilesHeight(2);
		m_Terrain->setDetailTextureFile(External(""));
		m_Terrain->setLatticeBaseName("balkanMap");
		m_Terrain->setLatticeElevationExtension("bmp");
		m_Terrain->setLatticeTextureExtension("bmp");
		m_Terrain->setTextureFile(External(""));
		m_Terrain->setElevationFile(External(""));
		m_Terrain->setVertexSpacing(250.0);
		m_Terrain->setVertexHeight(0.38);
		m_Terrain->setDetailThreshold(8);
		m_Terrain->setCenter(LLA(41.6450, 16.0057, 0.0));
		m_Terrain->setWidth(1024000);
		m_Terrain->setHeight(1024000);
		m_Terrain->setShader("terrain-demeter");
		m_Terrain->setUseFileTextureFactory(true);

		// This function is always called on serialized objects. Since we are
		// populating the object manually we also need to call this function.
		m_Terrain->postCreate();

		m_Terrain->activate();
		m_Terrain->setCameraPosition(-29495, -10530, 91.3);
		addChild(m_Terrain->getNode());
	}
	
private:
	Ref<DemeterTerrain> m_Terrain;
};

void addTerrain(csp::SDLViewer& /* sdlViewer */, osg::Group* parentNode) {
	parentNode->addChild(new TerrainGroup());
}
