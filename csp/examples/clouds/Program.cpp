/* OpenSceneGraph example, osgbillboard.
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

#include <iostream>

#include <osg/Node>
#include <osg/Geometry>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osg/Billboard>
#include <osg/LineWidth>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/StateSetManipulator>

#include <osgDB/Registry>
#include <osgDB/ReadFile>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <csp/csplib/data/Date.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/sky/Sky.h>
#include <csp/cspsim/sky/SkyGroup.h>
#include <csp/cspsim/weather/clouds/CloudBox.h>
#include <csp/cspsim/weather/clouds/CloudMath.h>
#include <csp/cspsim/weather/clouds/CloudRegistry.h>

typedef std::vector< osg::ref_ptr<osg::Image> > ImageList;

using namespace csp::weather::clouds;

/** create quad at specified position. */
osg::Drawable* createSquare(const osg::Vec3& corner,const osg::Vec3& width,const osg::Vec3& height)
{
    // set up the Geometry.
    osg::Geometry* geom = new osg::Geometry;

    osg::Vec3Array* coords = new osg::Vec3Array(4);
    (*coords)[0] = corner;
    (*coords)[1] = corner+width;
    (*coords)[2] = corner+width+height;
    (*coords)[3] = corner+height;

    geom->setVertexArray(coords);

    osg::Vec3Array* norms = new osg::Vec3Array(1);
    (*norms)[0] = width^height;
    (*norms)[0].normalize();
    
    geom->setNormalArray(norms);
    geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

    osg::Vec2Array* tcoords = new osg::Vec2Array(4);
    (*tcoords)[0].set(0.0f,0.0f);
    (*tcoords)[1].set(1.0f,0.0f);
    (*tcoords)[2].set(1.0f,1.0f);
    (*tcoords)[3].set(0.0f,1.0f);
    geom->setTexCoordArray(0,tcoords);
    
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));
    
    return geom;
}

osg::Drawable* createAxis(const osg::Vec3& corner,const osg::Vec3& xdir,const osg::Vec3& ydir,const osg::Vec3& zdir)
{
    // set up the Geometry.
    osg::Geometry* geom = new osg::Geometry;

    osg::Vec3Array* coords = new osg::Vec3Array(6);
    (*coords)[0] = corner;
    (*coords)[1] = corner+xdir;
    (*coords)[2] = corner;
    (*coords)[3] = corner+ydir;
    (*coords)[4] = corner;
    (*coords)[5] = corner+zdir;

    geom->setVertexArray(coords);

    osg::Vec4 x_color(0.0f,1.0f,1.0f,1.0f);
    osg::Vec4 y_color(0.0f,1.0f,1.0f,1.0f);
    osg::Vec4 z_color(1.0f,0.0f,0.0f,1.0f);

    osg::Vec4Array* color = new osg::Vec4Array(6);
    (*color)[0] = x_color;
    (*color)[1] = x_color;
    (*color)[2] = y_color;
    (*color)[3] = y_color;
    (*color)[4] = z_color;
    (*color)[5] = z_color;
    
    geom->setColorArray(color);
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,6));
    
    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(4.0f);
    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    geom->setStateSet(stateset);
    
    return geom;
}

void addModel(osg::Group* root)
{
    // add the drawable into a single goede to be shared...
    osg::Billboard* center = new osg::Billboard();
    center->setMode(osg::Billboard::POINT_ROT_EYE);
    center->addDrawable(
        createSquare(osg::Vec3(-0.5f,0.0f,-0.5f),osg::Vec3(1.0f,0.0f,0.0f),osg::Vec3(0.0f,0.0f,1.0f)),
        osg::Vec3(0.0f,0.0f,0.0f));
        
    osg::Billboard* x_arrow = new osg::Billboard();
    x_arrow->setMode(osg::Billboard::AXIAL_ROT);
    x_arrow->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
    x_arrow->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
    x_arrow->addDrawable(
       createSquare(osg::Vec3(-0.5f,0.0f,-0.5f),osg::Vec3(1.0f,0.0f,0.0f),osg::Vec3(0.0f,0.0f,1.0f)),
       osg::Vec3(5.0f,0.0f,0.0f));

    osg::Billboard* y_arrow = new osg::Billboard();
    y_arrow->setMode(osg::Billboard::AXIAL_ROT);
    y_arrow->setAxis(osg::Vec3(0.0f,1.0f,0.0f));
    y_arrow->setNormal(osg::Vec3(1.0f,0.0f,0.0f));
    y_arrow->addDrawable(
        createSquare(osg::Vec3(0.0f,-0.5f,-0.5f),osg::Vec3(0.0f,1.0f,0.0f),osg::Vec3(0.0f,0.0f,1.0f)),
        osg::Vec3(0.0f,5.0f,0.0f));

    osg::Billboard* z_arrow = new osg::Billboard();
    z_arrow->setMode(osg::Billboard::AXIAL_ROT);
    z_arrow->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
    z_arrow->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
    z_arrow->addDrawable(
        createSquare(osg::Vec3(-0.5f,0.0f,-0.5f),osg::Vec3(1.0f,0.0f,0.0f),osg::Vec3(0.0f,0.0f,1.0f)),
        osg::Vec3(0.0f,0.0f,5.0f));

    osg::Geode* axis = new osg::Geode();
    axis->addDrawable(createAxis(osg::Vec3(0.0f,0.0f,0.0f),osg::Vec3(5.0f,0.0f,0.0f),osg::Vec3(0.0f,5.0f,0.0f),osg::Vec3(0.0f,0.0f,5.0f)));

    root->addChild(center);
    root->addChild(x_arrow);
    root->addChild(y_arrow);
    root->addChild(z_arrow);
    root->addChild(axis);
}

void addRandomClouds(osg::Group* model, CloudRegistry::CloudVector& clouds, osg::Vec3 dimensions, int count) {
	CloudRegistry::CloudVector::size_type cloudIndex = 0;
	for(int i=0;i<count;++i, ++cloudIndex) {
		// Wrap around when there is no more clouds.
		if(cloudIndex >= clouds.size())
			cloudIndex = 0;

		float x = CloudMath::GenerateRandomNumber(0 - dimensions.x(), dimensions.x());
		float y = CloudMath::GenerateRandomNumber(0 - dimensions.y(), dimensions.y());
		float z = CloudMath::GenerateRandomNumber(0 - dimensions.z(), dimensions.z());

		osg::ref_ptr<osg::MatrixTransform> transformation = new osg::MatrixTransform();
		transformation->setMatrix(osg::Matrix::translate(x, y, z));
		transformation->addChild(clouds.at(cloudIndex).get());
		model->addChild(transformation.get());
	}
}

class Assert {
public:
	static void IsTrue(bool expression, const char* message) {
		if(!expression) WriteMessageAndAbort(message);
	}

	static void IsFalse(bool expression, const char* message) {
		if(expression) WriteMessageAndAbort(message);
	}

private:
	static void WriteMessageAndAbort(const char* message) {
		std::cout << message << std::endl;
		abort();
	}
};

/** This is the date and time for this application. */
double global_date = 0.0;

/** When we have significantly changed date and time we can set this variable to force
 * 	a complete update of the sky dome. */
bool force_update_of_sky_dome = false;

/** This class is responsible for updating stars, sun, moon and the sky dome
 *	for each frame. */
class SkyUpdateCallback : public osg::NodeCallback
{
	virtual void operator()(osg::Node* node, osg::NodeVisitor*)
	{
		csp::SkyGroup* skyGroup = dynamic_cast<csp::SkyGroup*>(node);
		if(skyGroup) {
			skyGroup->sky()->update(global_date, force_update_of_sky_dome);
			force_update_of_sky_dome = false;
		}
	}
};

void addSky(osg::Group* root) {
	/* Place the sky on its own camera to not disturb the near far
	   planes. */
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setRenderOrder(osg::Camera::PRE_RENDER);
	root->addChild(camera);

	// The sky group contains stars, sky dome, sun and moon.
	global_date = csp::SimDate(2008, 12, 31, 17, 59, 0).getJulianDate();
	osg::ref_ptr<csp::SkyGroup> skyGroup = new csp::SkyGroup();
	skyGroup->setUpdateCallback(new SkyUpdateCallback());
	camera->addChild(skyGroup.get());
}

osg::ref_ptr<osg::Group> addClouds(osg::Group* root) {
	osg::ref_ptr<osg::Group> cloudGroup = new osg::Group();
	root->addChild(cloudGroup.get());
	CloudRegistry::CloudVector clouds = CloudRegistry::CreateDefaultClouds();
	addRandomClouds(cloudGroup.get(), clouds, osg::Vec3(100, 100, 20), 40);
	return cloudGroup;
}

int main(int, char**) {
	std::string search_path = "./data/images/";
	csp::ObjectModel::setDataFilePathList(search_path);

	/* Ellipsoid tests. */
	Assert::IsFalse(CloudMath::InsideEllipsoid(osg::Vec3(1, 1, 1), osg::Vec3(0.5, 0.5, 0.5)), "Point shall be outside of sphere");
	Assert::IsFalse(CloudMath::InsideEllipsoid(osg::Vec3(1, 0, 0), osg::Vec3(0.5, 0.5, 0.5)), "Point shall be outside of sphere");
	Assert::IsFalse(CloudMath::InsideEllipsoid(osg::Vec3(0.3, 0.3, 1.0), osg::Vec3(0.5, 0.5, 0.5)), "Point shall be outside of sphere");
	Assert::IsFalse(CloudMath::InsideEllipsoid(osg::Vec3(0.3, 0.6, 0.3), osg::Vec3(0.5, 0.5, 0.5)), "Point shall be outside of sphere");
	Assert::IsTrue(CloudMath::InsideEllipsoid(osg::Vec3(0.3, 0.3, 0.3), osg::Vec3(0.5, 0.5, 0.5)), "Point shall be inside of sphere");
	Assert::IsTrue(CloudMath::InsideEllipsoid(osg::Vec3(-0.3, -0.3, -0.3), osg::Vec3(0.5, 0.5, 0.5)), "Point shall be inside of sphere");
	Assert::IsTrue(CloudMath::InsideEllipsoid(osg::Vec3(-0.3, 0.3, -0.3), osg::Vec3(0.5, 0.5, 0.5)), "Point shall be inside of sphere");
	Assert::IsTrue(CloudMath::InsideEllipsoid(osg::Vec3(0.49, 0.0, 0.0), osg::Vec3(0.5, 1.5, 1.5)), "Point shall be inside of sphere");

	/* Randomizer tests. */
	Assert::IsTrue(CloudMath::GenerateRandomNumber(0, 10) >= 0, "Should be a positive number");
	Assert::IsTrue(CloudMath::GenerateRandomNumber(-10, -1) < 0, "Should be a negative number");

    // construct the viewer
    osgViewer::Viewer viewer;
    viewer.addEventHandler(new osgViewer::StatsHandler());
    viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));

	// create the root node which will hold the model.
	osg::ref_ptr<osg::Group> root = new osg::Group();

	addSky(root.get());
	addModel(root.get());
	osg::ref_ptr<osg::Group> cloudGroup = addClouds(root.get());

	osg::ref_ptr<osgGA::TrackballManipulator> manipulator = new osgGA::TrackballManipulator();
	viewer.setCameraManipulator(manipulator.get());
    viewer.setSceneData(root.get());
	viewer.getCamera()->setClearMask(GL_DEPTH_BUFFER_BIT);
	viewer.realize();
	manipulator->setNode(cloudGroup.get());
	manipulator->home(0.0f);
    return viewer.run();
}
