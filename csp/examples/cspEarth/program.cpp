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
#include <osgDB/ReadFile>
#include <osg/Group>

#include <string>
#include <iostream>

using namespace csp;

/** This sample application will manually create a single object model
*  to be displayed in a scene. A object model represents basic info
*	about the 3d model loaded. 
*
*	Normally you won't need to create an object like this. All objects
*  are normally serialized directly from xml files.
*/

int main( int argc, char **argv )
{
	std::string osgEarthFileName = "D:/Dev/OSG/osgEarth/tests/srtm.earth";

	// use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc,argv);

	// Allow a debugger to be attached before the interesting parts start
	if (arguments.read("-p"))
	{
		std::cout << "Attach debugger and press a key...\n";
		std::getchar();
	}

	// if an .earth file definition was specified, use this one instead of the default
	arguments.read("-file", osgEarthFileName);

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

	std::cout << "reading osgEarth config file: " << osgEarthFileName << "\n";
	osg::ref_ptr<osg::Node> globe = osgDB::readNodeFile("D:/Dev/OSG/osgEarth/tests/srtm.earth");
	osg::ref_ptr<osg::Node> topNode = new osg::Node;

	osg::Group* group = new osg::Group;

	// =====================================================================
	// construct the viewer
	osgViewer::Viewer viewer;
	viewer.addEventHandler(new osgViewer::StatsHandler());
	viewer.addEventHandler(new osgViewer::HelpHandler());

	// Set scene data
	group->addChild(globe);
	group->addChild(my_model->getModel().get());

	// viewer.setSceneData(my_model->getModel().get());
	viewer.setSceneData(group);

	osgViewer::GraphicsWindowEmbedded* gfxWindow;
//	gfxWindow = viewer.setUpViewerAsEmbeddedInWindow(100, 100, 1000, 800);

	// run the viewers frame loop
	return viewer.run();
}
