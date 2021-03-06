#include <csp/cspsim/Config.h>
#include <csp/cspsim/ObjectModel.h>
#include <iostream>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <csp/modules/demeter/DemeterTerrain.h>

using namespace csp;

int main(int, char**) {
	try {
		std::string search_path = "./data/images/";
		csp::ObjectModel::setDataFilePathList(search_path);

		setDataPath("TerrainPath", "data/terrain");
		
		// These parameters is normally serialized from an xml file.
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

		terrain->activate();
		
		terrain->setCameraPosition(-29495, -10530, 91.3);
		
	    // construct the viewer
	    osgViewer::Viewer viewer;
	    viewer.addEventHandler(new osgViewer::StatsHandler());
		viewer.addEventHandler(new osgViewer::HelpHandler());
	    
		// Set scene data
	    viewer.setSceneData(terrain->getNode());

	    // run the viewers frame loop
	    return viewer.run();
	}
	catch(std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return -1;
}
