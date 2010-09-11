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
#include <csp/csplib/util/Ref.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/sky/Sky.h>
#include <csp/cspsim/sky/SkyGroup.h>
#include <csp/cspsim/weather/clouds/CloudModel.h>

typedef std::vector< osg::ref_ptr<osg::Image> > ImageList;

using namespace csp;
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

bool InsideEllipsoid(Vector3 point, Vector3 ellipsoid) {
	float value1 =
		(point.x() * point.x()) / (ellipsoid.x() * ellipsoid.x()) +
		(point.y() * point.y()) / (ellipsoid.y() * ellipsoid.y());
	float value2 =
		(point.x() * point.x()) / (ellipsoid.x() * ellipsoid.x()) +
		(point.z() * point.z()) / (ellipsoid.z() * ellipsoid.z());
	float value3 =
		(point.y() * point.y()) / (ellipsoid.y() * ellipsoid.y()) +
		(point.z() * point.z()) / (ellipsoid.z() * ellipsoid.z());

	return value1 < 1 && value2 < 1 && value3 < 1;
}

float GenerateRandomNumber(float minimum, float maximum) {
	return (rand() % static_cast<int>(maximum - minimum)) + minimum;
}

void addRandomClouds(osg::Group* model, std::vector<Ref<CloudModel> >& clouds, osg::Vec3 dimensions, int count) {
	std::vector<Ref<CloudModel> >::size_type cloudIndex = 0;
	for(int i=0;i<count;++i, ++cloudIndex) {
		// Wrap around when there is no more clouds.
		if(cloudIndex >= clouds.size())
			cloudIndex = 0;

		float x = GenerateRandomNumber(0 - dimensions.x(), dimensions.x());
		float y = GenerateRandomNumber(0 - dimensions.y(), dimensions.y());
		float z = GenerateRandomNumber(0 - dimensions.z(), dimensions.z());

		osg::ref_ptr<osg::MatrixTransform> transformation = new osg::MatrixTransform();
		transformation->setMatrix(osg::Matrix::translate(x, y, z));
		transformation->addChild(clouds.at(cloudIndex)->getNode());
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

void RemoveRedundantSprites(CloudBox* cloud_box, float threshold) {
	for(int j = cloud_box->m_Sprites.size()-1;j >= 0;--j) {
		Vector3 spritePosition = cloud_box->m_Sprites[j]->m_Position;
		bool deleteSprite = false;
		for(int k = cloud_box->m_Sprites.size()-1;k >= 0;--k) {
			// Don't compare with myself...
			if(j == k) {
				break;
			}

			Vector3 spritePositionToTest = cloud_box->m_Sprites[k]->m_Position;
			Vector3 delta = spritePositionToTest - spritePosition;
			float distance = sqrt(delta.x() * delta.x() + delta.y() * delta.y() + delta.z() * delta.z());

			// This value should be computed depending on the width and height of the sprite.
			if(distance < threshold) {
				deleteSprite = true;
				break;
			}
		}

		if(deleteSprite) {
			cloud_box->m_Sprites.erase(cloud_box->m_Sprites.begin() + j);
		}
	}
}

void CreateSprites(CloudBox* cloud_box, int density) {
	for(int i = 0;i<density;++i) {
		// Calculate the position of the sprite.
		Vector3 position(
			GenerateRandomNumber(0 - cloud_box->m_Size.x(), cloud_box->m_Size.x()),
			GenerateRandomNumber(0 - cloud_box->m_Size.y(), cloud_box->m_Size.y()),
			GenerateRandomNumber(0 - cloud_box->m_Size.z(), cloud_box->m_Size.z()));

		if(InsideEllipsoid(position, cloud_box->m_Size)) {
			Ref<CloudSprite> sprite = new CloudSprite();
			sprite->m_Position = position;
			sprite->m_Size = Vector2(GenerateRandomNumber(4, 9), GenerateRandomNumber(5, 7));
			sprite->m_Rotation = osg::DegreesToRadians(GenerateRandomNumber(0, 360));
			cloud_box->m_Sprites.push_back(Link<CloudSprite>(sprite.get()));
		}
	}
	RemoveRedundantSprites(cloud_box, 2.0);
}

void CreateDefaultClouds(std::vector<Ref<CloudModel> >& clouds) {
	/* Common objects shared by all clouds. */
	Ref<CloudLOD> lod = new CloudLOD();
	lod->m_Distance1 = 3000.0f;
	lod->m_Distance2 = 5000.0f;
	lod->m_Distance3 = 8000.0f;

	Ref<CloudColorLevels> color_levels = new CloudColorLevels();
	color_levels->addLevel(0.0f, Vector3(0.8f, 0.8f, 0.8f));
	color_levels->addLevel(0.25f, Vector3(0.82f, 0.82f, 0.82f));
	color_levels->addLevel(0.3f, Vector3(0.85f, 0.85f, 0.85f));
	color_levels->addLevel(0.4f, Vector3(0.87f, 0.87f, 0.87f));
	color_levels->addLevel(0.55f, Vector3(0.9f, 0.9f, 0.9f));
	color_levels->addLevel(0.6f, Vector3(0.93f, 0.93f, 0.93f));
	color_levels->addLevel(0.65f, Vector3(0.97f, 0.97f, 0.97f));
	color_levels->addLevel(0.7f, Vector3(1.0f, 1.0f, 1.0f));

	Ref<CloudOpacityLevels> opacity_levels = new CloudOpacityLevels();
	opacity_levels->addLevel(0.0f, 0.1f);
	opacity_levels->addLevel(0.1f, 0.3f);
	opacity_levels->addLevel(0.3f, 0.7f);
	opacity_levels->addLevel(0.4f, 1.0f);

	Ref<CloudModel> cloud = new CloudModel();
	Ref<CloudBox> cloud_box = new CloudBox();
	cloud_box->m_Size = Vector3(15.0f, 15.0f, 5.0f);
	cloud_box->m_ColorLevels = color_levels.get();
	cloud_box->m_OpacityLevels = opacity_levels.get();
	cloud_box->m_LOD = lod.get();
	CreateSprites(cloud_box.get(), 250);
	cloud->m_CloudBoxes.push_back(Link<CloudBox>(cloud_box.get()));
	cloud->m_CloudBoxPositions.push_back(Vector3(0, 0, 0));
	clouds.push_back(cloud);

	cloud = new CloudModel();
	cloud_box = new CloudBox();
	cloud_box->m_Size = Vector3(20.0f, 15.0f, 5.0f);
	cloud_box->m_ColorLevels = color_levels.get();
	cloud_box->m_OpacityLevels = opacity_levels.get();
	cloud_box->m_LOD = lod.get();
	CreateSprites(cloud_box.get(), 300);
	cloud->m_CloudBoxes.push_back(Link<CloudBox>(cloud_box.get()));
	cloud->m_CloudBoxPositions.push_back(Vector3(0, 0, 0));

	cloud_box = new CloudBox();
	cloud_box->m_Size = Vector3(5.0f, 5.0f, 8.0f);
	cloud_box->m_ColorLevels = color_levels.get();
	cloud_box->m_OpacityLevels = opacity_levels.get();
	cloud_box->m_LOD = lod.get();
	CreateSprites(cloud_box.get(), 100);
	cloud->m_CloudBoxes.push_back(Link<CloudBox>(cloud_box.get()));
	cloud->m_CloudBoxPositions.push_back(Vector3(0, 0, 0));
	clouds.push_back(cloud);
}

osg::ref_ptr<osg::Group> addClouds(osg::Group* root) {
	osg::ref_ptr<osg::Group> cloudGroup = new osg::Group();
	root->addChild(cloudGroup.get());
	std::vector<Ref<CloudModel> > clouds;
	CreateDefaultClouds(clouds);
	addRandomClouds(cloudGroup.get(), clouds, osg::Vec3(100, 100, 20), 40);
	return cloudGroup;
}

int main(int, char**) {
	std::string search_path = "./data/images/";
	csp::ObjectModel::setDataFilePathList(search_path);

	/* Ellipsoid tests. */
	Assert::IsFalse(InsideEllipsoid(Vector3(1, 1, 1), Vector3(0.5, 0.5, 0.5)), "Point shall be outside of sphere");
	Assert::IsFalse(InsideEllipsoid(Vector3(1, 0, 0), Vector3(0.5, 0.5, 0.5)), "Point shall be outside of sphere");
	Assert::IsFalse(InsideEllipsoid(Vector3(0.3, 0.3, 1.0), Vector3(0.5, 0.5, 0.5)), "Point shall be outside of sphere");
	Assert::IsFalse(InsideEllipsoid(Vector3(0.3, 0.6, 0.3), Vector3(0.5, 0.5, 0.5)), "Point shall be outside of sphere");
	Assert::IsTrue(InsideEllipsoid(Vector3(0.3, 0.3, 0.3), Vector3(0.5, 0.5, 0.5)), "Point shall be inside of sphere");
	Assert::IsTrue(InsideEllipsoid(Vector3(-0.3, -0.3, -0.3), Vector3(0.5, 0.5, 0.5)), "Point shall be inside of sphere");
	Assert::IsTrue(InsideEllipsoid(Vector3(-0.3, 0.3, -0.3), Vector3(0.5, 0.5, 0.5)), "Point shall be inside of sphere");
	Assert::IsTrue(InsideEllipsoid(Vector3(0.49, 0.0, 0.0), Vector3(0.5, 1.5, 1.5)), "Point shall be inside of sphere");

	/* Randomizer tests. */
	Assert::IsTrue(GenerateRandomNumber(0, 10) >= 0, "Should be a positive number");
	Assert::IsTrue(GenerateRandomNumber(-10, -1) < 0, "Should be a negative number");

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
