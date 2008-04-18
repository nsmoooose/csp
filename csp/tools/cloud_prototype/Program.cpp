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

#pragma comment(lib, "osg")
#pragma comment(lib, "osgDB")
#pragma comment(lib, "osgGA")
#pragma comment(lib, "osgViewer")

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

#include <osgDB/Registry>
#include <osgDB/ReadFile>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include "CloudBox.h"
#include "CloudMath.h"
#include "CloudRegistry.h"

typedef std::vector< osg::ref_ptr<osg::Image> > ImageList;

using namespace csp::clouds;

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

osg::Group* createModel()
{
    // create the root node which will hold the model.
    osg::Group* root = new osg::Group();

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

    return root;
}

void addClouds(osg::Group* model, CloudRegistry::CloudVector& clouds, float radius, int count) {
	CloudRegistry::CloudVector::size_type cloudIndex = 0;

	float angle = osg::PI * 2 / count;
	for(int i = 0;i<count;++i, ++cloudIndex) {
		// Wrap around when there is no more clouds.
		if(cloudIndex >= clouds.size()) 
			cloudIndex = 0;

		float x = cos(static_cast<float>(i) * angle) * radius;
		float y = sin(static_cast<float>(i) * angle) * radius;
		float z = sin(static_cast<float>(i) * angle * 3) * 10;

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

int main(int, char**) {
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
    
    // Create a x, y, z model.
	osg::ref_ptr<osg::Group> model = createModel();

	CloudRegistry::CloudVector clouds = CloudRegistry::CreateDefaultClouds();
	// Add clouds
	addClouds(model.get(), clouds, 50, 10);
	addClouds(model.get(), clouds, 100, 18);

	// Set scene data
    viewer.setSceneData(model.get());

    // run the viewers frame loop
    return viewer.run();
}
