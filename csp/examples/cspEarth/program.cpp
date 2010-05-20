/* OpenSceneGraph example, osglight.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

#include <csp/csplib/util/FileUtility.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/Shader.h>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/ViewerBase>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgGA/FlightManipulator>
#include <osg/Group>

#include <string>
#include <iostream>

#include "InputHandler.h"

using namespace csp;

osgViewer::Viewer viewer;

/** This sample application will manually create a single object model
*  to be displayed in a scene. A object model represents basic info
*	about the 3d model loaded. 
*
*	Normally you won't need to create an object like this. All objects
*  are normally serialized directly from xml files.
*/

void setupCameraAndContext(osgViewer::Viewer& viewer, int windowWidth, int windowHeight)
{
    osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
    if (!wsi) 
    {
        osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot create windows."<<std::endl;
        return;
    }
    
    unsigned int width, height;
    wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);

	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x = 0;
	traits->y = 0;
	// use either system native screen size or what was specified in command line
	if (windowWidth > 0 && windowHeight > 0)
	{
		traits->width = windowWidth;
		traits->height = windowHeight;
		traits->x = 50;
		traits->y = 50;
	}
	else
	{
		traits->width = width;
		traits->height = height;
	}
    traits->windowDecoration = true;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;

    osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
    if (gc.valid())
    {
        osg::notify(osg::INFO)<<"  GraphicsWindow has been created successfully."<<std::endl;

        // need to ensure that the window is cleared make sure that the complete window is set the correct colour
        // rather than just the parts of the window that are under the camera's viewports
        gc->setClearColor(osg::Vec4f(0.2f,0.2f,0.6f,1.0f));
        gc->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    else
    {
        osg::notify(osg::NOTICE)<<"  GraphicsWindow has not been created successfully."<<std::endl;
    }

    unsigned int numCameras = 1;
    double aspectRatioScale = 1.0;///(double)numCameras;
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setGraphicsContext(gc.get());
	if (windowWidth > 0 && windowHeight > 0)
		camera->setViewport(new osg::Viewport(0, 0, windowWidth, windowHeight));
	else
		camera->setViewport(new osg::Viewport(0, 0, width, height));
    GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
    camera->setDrawBuffer(buffer);
    camera->setReadBuffer(buffer);

    viewer.addSlave(camera.get(), osg::Matrixd(), osg::Matrixd::scale(aspectRatioScale,1.0,1.0));
}

void dumpSceneToDisk()
{
	std::string filename = "dump";
	osgViewer::ViewerBase::Scenes viewerScenes;
	osgViewer::Scene* scene;

	viewer.getScenes(viewerScenes, true);
	for (osgViewer::ViewerBase::Scenes::iterator it = viewerScenes.begin(); it!=viewerScenes.end(); ++it)
	{
		scene = *it;
		osg::Node* node = scene->getSceneData();

		bool result = osgDB::writeNodeFile(*node, filename + ".osg" );

		if ( !result )
			osg::notify(osg::FATAL) << "Failed in osgDB::writeNodeFile()." << std::endl;
	}
}

int main( int argc, char **argv )
{
	int windowWidth, windowHeight;
	std::string osgEarthFileName = "D:/Dev/csp/csp/examples/cspEarth/data/srtm.earth";

	// use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc,argv);

	// Allow a debugger to be attached before the interesting parts start
	if (arguments.read("-p"))
	{
		std::cout << "Attach debugger and press a key..." << std::endl;
		std::getchar();
	}

	// if an .earth file definition was specified, use this one instead of the default
	arguments.read("-file", osgEarthFileName);

	// get screen size
	arguments.read("-window", windowWidth, windowHeight);

	// =====================================================================
	// SETUP
	// Make it possible for this application to locate images bound
	// to textures.
	std::string search_path = ";";
	csp::ospath::addpath(search_path, "./data/images/");
	csp::ospath::addpath(search_path, "./data/models/");
	csp::ObjectModel::setDataFilePathList(search_path);

	// Set paths for shader files.
	Shader::instance()->setShaderPath("./data/shaders/");


	// =====================================================================
	// Manual creation of a object model.
	Ref<ObjectModel> my_model = new ObjectModel();

	// An external is used to handle the path to the scene graph file.
	// It is also ensuring that paths is handled independent on operating
	// system.
	External modelPath;
	modelPath.setSource("industry/refinery_column01/refinery_column01.osg");
	my_model->setModelPath(modelPath);
	my_model->setSmooth(true);

	// Load the model from disc. This will also apply needed shaders.
	// smoothing and more things.
	my_model->loadModel();

	std::cout << "reading osgEarth config file: " << osgEarthFileName << std::endl;
	osg::ref_ptr<osg::Node> globe = osgDB::readNodeFile(osgEarthFileName);
	osg::ref_ptr<osg::Node> topNode = new osg::Node;

	osg::Group* group = new osg::Group;

	// =====================================================================
	// construct the viewer
	viewer.addEventHandler(new osgViewer::StatsHandler());
	viewer.addEventHandler(new osgViewer::HelpHandler());

	// Set scene data
	group->addChild(globe.get());
	group->addChild(my_model->getModel().get());

	//viewer.setSceneData(my_model->getModel().get());
	viewer.setSceneData(group);
	//viewer.setSceneData(globe.get());

	// create camera and context
	setupCameraAndContext(viewer, windowWidth, windowHeight);

	//dumpSceneToDisk(group, "test");
	//dumpSceneToDisk(globe.get(), "globe");
	//dumpSceneToDisk(my_model->getModel().get(), "object");

	viewer.setCameraManipulator(new osgGA::FlightManipulator);

	viewer.addEventHandler(new inputHandler());
	// run the viewers frame loop
    viewer.realize();

    // main loop (note, window toolkits which take control over the main loop will require a window redraw callback containing the code below.)
    while(!viewer.done())
    {
        viewer.frame();
    }
}
