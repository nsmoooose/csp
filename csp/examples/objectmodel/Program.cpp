#include <csp/csplib/util/FileUtility.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/Shader.h>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

using namespace csp;

/** This sample application will manually create a single object model
 *  to be displayed in a scene. A object model represents basic info
 *	about the 3d model loaded. 
 *
 *	Normally you won't need to create an object like this. All objects
 *  are normally serialized directly from xml files.
 */

int main(int, char**) {
	// =====================================================================
	// SETUP
	// Make it possible for this application to locate images bound
	// to textures.
	std::string search_path;
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



	// =====================================================================
    // construct the viewer
    osgViewer::Viewer viewer;
    viewer.addEventHandler(new osgViewer::StatsHandler());
	viewer.addEventHandler(new osgViewer::HelpHandler());
    
	// Set scene data
    viewer.setSceneData(my_model->getModel().get());

    // run the viewers frame loop
    return viewer.run();
}
