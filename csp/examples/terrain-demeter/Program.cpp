#include <csp/cspsim/ObjectModel.h>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <csp/modules/demeter/DemeterTerrain.h>

using namespace csp;

int main(int, char**) {
	std::string search_path = "./data/images/";
	csp::ObjectModel::setDataFilePathList(search_path);

	Ref<DemeterTerrain> terrain = new DemeterTerrain();

	terrain->setName("Balkan Terrain");
	terrain->setVersion(0);
	terrain->setUseDynamicTextures(false);
	terrain->setUseTextureCompression(true);
	terrain->setPreloadTextures(true);
	terrain->setUseTextureFactory(false);
	terrain->setMaxTriangles(60000);
	terrain->setLattice(true);
	terrain->setLatticeWidth(16);
	terrain->setLatticeHeight(16);
    terrain->setLatticeTilesWidth(2);
    terrain->setLatticeTilesHeight(2);
	terrain->setDetailTextureFile(External(""));
	terrain->setLatticeBaseName("balkanMap");
	terrain->setLatticeElevationExtension("bmp");
	terrain->setLatticeTextureExtension("bmp");
	terrain->setTextureFile(External(""));
	terrain->setElevationFile(External(""));
	terrain->setVertexSpacing(250.0);
	terrain->setVertexHeight(0.38);
	terrain->setDetailThreshold(8);
	terrain->setCenter(LLA(41.6450, 16.0057, 0.0));
	terrain->setWidth(1024000);
	terrain->setHeight(1024000);
	terrain->setShader("terrain-demeter");
    terrain->setUseFileTextureFactory(true);

	// This function is always called on serialized objects. Since we are
	// populating the object manually we also need to call this function.
	terrain->postCreate();

    // construct the viewer
    osgViewer::Viewer viewer;
    viewer.addEventHandler(new osgViewer::StatsHandler());
	viewer.addEventHandler(new osgViewer::HelpHandler());
    
	// Set scene data
    viewer.setSceneData(terrain->getNode());

    // run the viewers frame loop
    return viewer.run();
}
