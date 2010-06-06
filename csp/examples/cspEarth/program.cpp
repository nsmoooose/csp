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
#include <osg/NodeCallback>
#include <osgEarthUtil/Viewpoint>
#include <osgEarthUtil/EarthManipulator>

#include <string>
#include <iostream>

#include "InputHandler.h"
#include "ShowNodeNamesVisitor.h"

using namespace csp;

osgViewer::Viewer viewer;
osgEarthUtil::EarthManipulator* manip;

/** This sample application will manually create a single object model
*  to be displayed in a scene. A object model represents basic info
*	about the 3d model loaded. 
*
*	Normally you won't need to create an object like this. All objects
*  are normally serialized directly from xml files.
*/

class StatsOverlayCallback : public osg::NodeCallback
{
public:
	StatsOverlayCallback() : _counter( 0. ) {}

	virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
	{
		std::cout<<"Camera update callback - pre traverse"<<node<<std::endl;
		_counter += .1;
		traverse( node, nv );
	}

protected:
	float _counter;
};

//class InsertCallbacksVisitor : public osg::NodeVisitor
//{
//
//   public:
//   
//        InsertCallbacksVisitor():osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
//        {
//        }
//        
//        virtual void apply(osg::Node& node)
//        {
////             node.setUpdateCallback(new UpdateCallback());
////             node.setCullCallback(new CullCallback());
//             traverse(node);
//        }
//
//        virtual void apply(osg::Geode& geode)
//        {
////            geode.setUpdateCallback(new UpdateCallback());
//			geode.setUpdateCallback(new StatsOverlayCallback());
//            
//            //note, it makes no sense to attach a cull callback to the node
//            //at there are no nodes to traverse below the geode, only
//            //drawables, and as such the Cull node callbacks is ignored.
//            //If you wish to control the culling of drawables
//            //then use a drawable cullback...
//
//            for(unsigned int i=0;i<geode.getNumDrawables();++i)
//            {
////                geode.getDrawable(i)->setUpdateCallback(new DrawableUpdateCallback());
////                geode.getDrawable(i)->setCullCallback(new DrawableCullCallback());
////                geode.getDrawable(i)->setDrawCallback(new DrawableDrawCallback());
//            }
//        }
//        
//        virtual void apply(osg::Transform& node)
//        {
//            apply((osg::Node&)node);
//        }
//};

void showNodeNames()
{
	osgViewer::ViewerBase::Scenes viewerScenes;
	osgViewer::Scene* scene;
	ShowNodeNamesVisitor snnv( true );

	viewer.getScenes(viewerScenes, true);
	for (osgViewer::ViewerBase::Scenes::iterator it = viewerScenes.begin(); it!=viewerScenes.end(); ++it)
	{
		scene = *it;
		osg::Node* node = scene->getSceneData();
		node->accept( snnv );
	}
}

osg::Camera* createStatsOverlay()
{
    // create a camera to set up the projection and model view matrices, and the subgraph to draw in the HUD
    osg::Camera* camera = new osg::Camera;

    // set the projection matrix
    camera->setProjectionMatrix(osg::Matrix::ortho2D(0,1280,0,1024));

    // set the view matrix    
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setViewMatrix(osg::Matrix::identity());

    // only clear the depth buffer
    camera->setClearMask(GL_DEPTH_BUFFER_BIT);

    // draw subgraph after main camera view.
    camera->setRenderOrder(osg::Camera::POST_RENDER);

    // we don't want the camera to grab event focus from the viewers main camera(s).
    camera->setAllowEventFocus(false);
    


    // add to this camera a subgraph to render
    {

        osg::Geode* geode = new osg::Geode();
		geode->setDataVariance( osg::Object::DYNAMIC );

        std::string timesFont("fonts/arial.ttf");

        // turn lighting off for the text and disable depth test to ensure it's always ontop.
        osg::StateSet* stateset = geode->getOrCreateStateSet();
        stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

        osg::Vec3 position(150.0f,800.0f,0.0f);
        osg::Vec3 delta(0.0f,-120.0f,0.0f);

        osgText::Text* text = new  osgText::Text;
		text->setDataVariance( osg::Object::DYNAMIC );
        geode->addDrawable( text );

        text->setFont(timesFont);
        text->setPosition(position);
        text->setText("And finally set the Camera's RenderOrder to POST_RENDER\n"
                      "to make sure it's drawn last.");

        position += delta;
        
        {
            osg::BoundingBox bb;
            for(unsigned int i=0;i<geode->getNumDrawables();++i)
            {
                bb.expandBy(geode->getDrawable(i)->getBound());
            }

            osg::Geometry* geom = new osg::Geometry;

            osg::Vec3Array* vertices = new osg::Vec3Array;
            float depth = bb.zMin()-0.1;
            vertices->push_back(osg::Vec3(bb.xMin(),bb.yMax(),depth));
            vertices->push_back(osg::Vec3(bb.xMin(),bb.yMin(),depth));
            vertices->push_back(osg::Vec3(bb.xMax(),bb.yMin(),depth));
            vertices->push_back(osg::Vec3(bb.xMax(),bb.yMax(),depth));
            geom->setVertexArray(vertices);

            osg::Vec3Array* normals = new osg::Vec3Array;
            normals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
            geom->setNormalArray(normals);
            geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

            osg::Vec4Array* colors = new osg::Vec4Array;
            colors->push_back(osg::Vec4(1.0f,1.0,0.8f,0.2f));
            geom->setColorArray(colors);
            geom->setColorBinding(osg::Geometry::BIND_OVERALL);

            geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS,0,4));

            osg::StateSet* stateset = geom->getOrCreateStateSet();
            stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
            //stateset->setAttribute(new osg::PolygonOffset(1.0f,1.0f),osg::StateAttribute::ON);
            stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

            geode->addDrawable(geom);
        }

		geode->setUpdateCallback(new StatsOverlayCallback());
        camera->addChild(geode);
	}

    return camera;
}

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

	// create HUD camera
	osg::Camera* overlayCamera = createStatsOverlay();
    overlayCamera->setGraphicsContext(gc.get());
    overlayCamera->setViewport(0,0,gc.get()->getTraits()->width, gc.get()->getTraits()->height);

    viewer.addSlave(overlayCamera, false);
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

osgEarthUtil::EarthManipulator* getManipulator()
{
	return manip;
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
	group->setName("root group");

	// insert all the callbacks
    //InsertCallbacksVisitor icv;
    //group->accept(icv);

	// =====================================================================
	// construct the viewer
	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.addEventHandler(new osgViewer::HelpHandler);
    viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::ScreenCaptureHandler);

	osgViewer::View::EventHandlers eventHandlers = viewer.getEventHandlers();
	// iterate through the viewer's event handlers and modify their default behavior
	for (osgViewer::View::EventHandlers::iterator it = eventHandlers.begin(); it != eventHandlers.end(); ++it)
	{
		// EventHandlers is a list of osgGA::GUIEventHandlers, so RTTI is used to find out the derived class
		if(osgViewer::WindowSizeHandler *winSizeHandler = 
			dynamic_cast<osgViewer::WindowSizeHandler *>(it->get()))
		{
			winSizeHandler->setKeyEventToggleFullscreen(osgGA::GUIEventAdapter::KEY_F2);
		}
	}


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

	//viewer.setCameraManipulator(new osgGA::FlightManipulator);

	osgEarth::MapNode* mapNode = osgEarth::MapNode::findMapNode(globe);
    manip = new osgEarthUtil::EarthManipulator();
	manip->setNode(globe);
    if ( mapNode->getMap()->isGeocentric() )
    {
        manip->setHomeViewpoint( 
            osgEarthUtil::Viewpoint( osg::Vec3d( -90, 0, 0 ), 0.0, -90.0, 5e7 ) );
    }
	manip->getSettings()->bindMouseDoubleClick(
		osgEarthUtil::EarthManipulator::ACTION_GOTO, osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON );

	viewer.setCameraManipulator(manip);
	viewer.addEventHandler(new inputHandler());
	// run the viewers frame loop
    viewer.realize();

    // main loop (note, window toolkits which take control over the main loop will require a window redraw callback containing the code below.)
    while(!viewer.done())
    {
        viewer.frame();
    }
}
