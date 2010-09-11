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

#include <csp/csplib/data/DataArchive.h>
#include <csp/csplib/data/DataManager.h>
#include <csp/csplib/util/Ref.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/RegisterObjectInterfaces.h>
#include <csp/cspsim/theater/ElevationCorrection.h>
#include <csp/cspsim/theater/FeatureSceneGroup.h>
#include <csp/cspsim/theater/LayoutTransform.h>
#include <csp/cspsim/weather/clouds/CloudModel.h>
#include <csp/cspsim/weather/clouds/CloudCoverModel.h>

using namespace csp;
using namespace csp::weather::clouds;

void addClouds(DataManager* dm, FeatureSceneGroup* root) {
	Ref<CloudCoverModel> cover = dm->getObject("cloud_scene:scene.master");
	LayoutTransform transform;
	ElevationCorrection correction(NULL, 0, 0, 0);
	cover->addSceneModel(root, transform, correction);
}

int main(int, char**) {
	registerAllObjectInterfaces();

	std::string search_path = "./data/images/";
	csp::ObjectModel::setDataFilePathList(search_path);

	Ref<DataManager> data_manager = new DataManager();
	DataArchive* archive = new DataArchive("examples/clouds-scene/cloud_scene.dar", true);
	data_manager->addArchive(archive);

	// construct the viewer
	osgViewer::Viewer viewer;
	viewer.addEventHandler(new osgViewer::StatsHandler());
	viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));

	// create the root node which will hold the model.
	osg::ref_ptr<FeatureSceneGroup> root = new FeatureSceneGroup();
	addClouds(data_manager.get(), root.get());
	viewer.setSceneData(root.get());
	return viewer.run();
}
