#include <osg/Depth>
#include <osg/Geode>
#include <osg/StateSet>

#include "LogStream.h"
#include "Platform.h"
#include "TerrainObject.h"
#include "VirtualBattlefield.h"

extern int g_ScreenWidth;
extern int g_ScreenHeight;

extern VirtualBattlefield * g_pBattlefield;


// Terrain Object is a wrapper class for the Demeter terrain engine
// and DemeterDrawable OSG class.

// The class contains all the parameters used by the Terrain Engine.
// When the SetActive function is called the Terrain Engine will be
// Initialized and a Drawable node will be added to the scene graph.

/*! 
\fn TerrainObject::TerrainObject()
Default constructor for the Terrain Object. Sets default values.
*/
TerrainObject::TerrainObject()
{

	m_pTerrainLattice = NULL;
	m_pTerrain = NULL;
	m_pTerrainTextureFactory = NULL;

	m_pDrawable = NULL;
	m_pLatticeDrawable = NULL;

	m_bDynamicTextures = false;
	m_bTextureCompression = true;
	m_bPreloadTextures = true;
	m_bTextureFactory = false;
	m_iMaxTriangles = 50000;
	
	std::string m_sDetailTextureFile = "";
	std::string m_sTextureFile = "";
	std::string m_sElevationFile = "";
	
	m_fVertexSpacing = 1.0;
	m_fVertexHeight = 1.0;
	m_bLattice = false;
	m_bActive = false;
	m_fDetailThreshold = 12.0;

	m_sLatticeBaseName = "";
	m_sLatticeElevExt = "";
	m_sLatticeTexExt = "";
	m_sLatticeWidth = 0;
	m_sLatticeHeight = 0;


}

TerrainObject::~TerrainObject()
{
	if (m_pTerrainLattice)
		delete m_pTerrainLattice;
	if (m_pTerrain)
		delete m_pTerrain;
	if (m_pTerrainTextureFactory)
		delete m_pTerrainTextureFactory;
}

/*! 
\fn void TerrainObject::SetActive(bool value)
Used to activiate the terrain engine.
*/
void TerrainObject::SetActive(bool value)
{
	if (value && !m_bActive)
	{
		m_bActive = true;
		if (m_bLattice)
		{
			// create lattice
			Demeter::Settings::GetInstance()->SetMediaPath("../Data/Terrain");
			createTerrainLattice();
		    osg::Node * pTerrainLatticeNode = CreateTerrainLatticeNode(m_pTerrainLattice);
			pTerrainLatticeNode->setName("TerrainLatticeNode");
//			m_pLatticeDrawable  = pTerrainLatticeNode->getDrawable(0);
			g_pBattlefield->addNodeToScene(pTerrainLatticeNode);
			g_pBattlefield->setActiveTerrain(this);

		}
		else
		{
			// create single terrain node
			Demeter::Settings::GetInstance()->SetMediaPath("../Data/Terrain");
			createTerrain();
			osg::Node * pTerrainNode = CreateTerrainNode(m_pTerrain);
			pTerrainNode->setName("TerrainNode");
//			m_pDrawable = pTerrainNode->getDrawable(0);
			g_pBattlefield->addNodeToScene(pTerrainNode);
			g_pBattlefield->setActiveTerrain(this);
		}
	}

}

float TerrainObject::GetElevation(float x, float y)
{
	if (!m_bLattice)
	{
		if (m_pTerrain)
			return m_pTerrain->GetElevation(x, y);
	}
	else
	{
		if (m_pTerrainLattice)
			return m_pTerrainLattice->GetElevation(x,y);
	}
	return 0.0;

}

void TerrainObject::GetNormal(float x, float y, float & normalX, 
		float & normalY, float & normalZ )
{
	normalX = 0; normalY = 0; normalZ = 1;
	if (!m_bLattice)
	{
		if (m_pTerrain)
			m_pTerrain->GetNormal(x, y, normalX, normalY, normalZ);
	}
	else
	{
		if (m_pTerrainLattice)
			m_pTerrainLattice->GetNormal(x,y,normalX,normalY,normalZ);
	}

}



int TerrainObject::createTerrain()
{
    CSP_LOG(CSP_ALL, CSP_DEBUG, "VirtualBattlefield::createTerrain() " );

	UpdateDemeterSettings();

	if (!m_bTextureFactory)
	{
    
		if (m_sDetailTextureFile == "")
			m_pTerrain = new Demeter::Terrain(m_sElevationFile.c_str(),m_sTextureFile.c_str(),NULL,
	   			  m_fVertexSpacing,m_fVertexHeight,m_iMaxTriangles);
		else
			m_pTerrain = new Demeter::Terrain(m_sElevationFile.c_str(),m_sTextureFile.c_str(),
				  m_sDetailTextureFile.c_str(),m_fVertexSpacing,m_fVertexHeight,m_iMaxTriangles);

	}
	else
	{
		if (m_sDetailTextureFile == "")
			m_pTerrain = new Demeter::Terrain(m_sElevationFile.c_str(),  NULL ,
				  m_sDetailTextureFile.c_str() ,m_fVertexSpacing,m_fVertexHeight,m_iMaxTriangles, false, 0, 0, 4,4);
		else 
			m_pTerrain = new Demeter::Terrain(m_sElevationFile.c_str(),  NULL ,
				  NULL ,m_fVertexSpacing,m_fVertexHeight,m_iMaxTriangles, false, 0, 0, 4,4);

   
		m_pTerrainTextureFactory = new Demeter::TerrainTextureFactory();
		m_pTerrainTextureFactory->SetTerrain(m_pTerrain);

		m_pTerrain->SetTextureFactory(m_pTerrainTextureFactory);
	}

	m_pTerrain->SetDetailThreshold(m_fDetailThreshold);



    return 1;
}

int TerrainObject::createTerrainLattice()
{

    CSP_LOG(CSP_ALL, CSP_DEBUG, "TerrainObject::createTerrainLattice()..." );

    //float detailThreshold = Config.GetFloat("TerraindetailThreshold");
    


	UpdateDemeterSettings();
	
	//char terrainTextureDetail[256];
//	Config.GetString(terrainTextureDetail, "TerrainTextureDetail");

//	if (m_sDetailTextureFile == "")
//	{
//		m_pTerrainLattice = new Demeter::TerrainLattice(m_sLatticeBaseName.c_str(),
//			m_sLatticeElevExt.c_str(), m_sLatticeTexExt.c_str(),		
//			NULL, m_fVertexSpacing, m_fVertexHeight, m_iMaxTriangles,
//			false, m_sLatticeWidth, m_sLatticeHeight);
//	}
//	else
//	{
//		m_pTerrainLattice = new Demeter::TerrainLattice(m_sLatticeBaseName.c_str(),
//			m_sLatticeElevExt.c_str(), m_sLatticeTexExt.c_str(),		
//			m_sDetailTextureFile.c_str(), m_fVertexSpacing, m_fVertexHeight, m_iMaxTriangles,
//			false, m_sLatticeWidth, m_sLatticeHeight);
//
//	}


	m_pTerrainLattice = new Demeter::TerrainLattice(m_sLatticeBaseName.c_str(),
			m_sLatticeElevExt.c_str(), /*m_sLatticeTexExt.c_str() */ NULL,		
			m_sDetailTextureFile.c_str(), m_fVertexSpacing, m_fVertexHeight, m_iMaxTriangles,
			true, m_sLatticeWidth, m_sLatticeHeight);
	
	m_pTerrainTextureFactory = new Demeter::TerrainTextureFactory();
	m_pTerrainTextureFactory->SetTerrainLattice(m_pTerrainLattice);
	m_pTerrainLattice->SetTextureFactory(m_pTerrainTextureFactory, 2, 2);

	m_pTerrainLattice->SetDetailThreshold(m_fDetailThreshold);
  
	m_pTerrainLattice->SetCameraPosition(500000, 500000, 3000);

	CSP_LOG(CSP_ALL, CSP_DEBUG, " ...TerrainObject::createTerrainLattice()");

	return 1;

	
}

void TerrainObject::setCameraPosition(float x, float y, float z)
{
	if (m_pTerrainLattice)
		m_pTerrainLattice->SetCameraPosition(x, y, z);
		

}


osg::Node* TerrainObject::CreateTerrainLatticeNode(Demeter::TerrainLattice* pTerrainLattice)
{
    CSP_LOG(CSP_APP, CSP_INFO, "TerrainObject::CreateTerrainLatticeNode" );
    
	Demeter::DemeterLatticeDrawable* pLatticeDrawable = NULL;
    osg::Geode* pGeode = NULL;


    try
    {
//		osg::Depth* depth = new osg::Depth;
//		depth->setFunction(osg::Depth::LEQUAL);
//		osg::StateSet *dstate = new osg::StateSet;
//		dstate->setAttributeAndModes(depth,osg::StateAttribute::ON );

        pLatticeDrawable = new Demeter::DemeterLatticeDrawable;
        pLatticeDrawable->SetTerrainLattice(pTerrainLattice);

//		pLatticeDrawable->setStateSet( dstate );

		pGeode = osgNew osg::Geode;
		pGeode->addDrawable(pLatticeDrawable);
    }
    catch(...)
    {
        CSP_LOG(CSP_APP, CSP_ERROR, "Caught Exception in VirtualBattlefield::CreateTerrainLatticeNode");
    }
    
    return pGeode;
}


osg::Node* TerrainObject::CreateTerrainNode(Demeter::Terrain* pTerrain)
{
    CSP_LOG(CSP_APP, CSP_INFO, "TerrainObject::CreateTerrainNode" );
    
    osg::Geode* pGeode = NULL;

    try
    {
        Demeter::DemeterDrawable* pDrawable = new Demeter::DemeterDrawable;
        pDrawable->SetTerrain(pTerrain);
        pGeode = osgNew osg::Geode;
        pGeode->addDrawable(pDrawable);
    }
    catch(...)
    {
        CSP_LOG(CSP_APP, CSP_ERROR, "Caught Exception in VirtualBattlefield::CreateTerrainNode");
    }
    
    return pGeode;
}

int TerrainObject::getTerrainPolygonsRendered()
{
	if (m_pTerrainLattice)
		return m_pTerrainLattice->GetLatticePolygonsRendered();
	else if (m_pTerrain)
		return m_pTerrain->GetTerrainPolygonsRendered();
	else 
		return 0;

}


void TerrainObject::UpdateDemeterSettings()
{
    std::string mediaPath = OSPath::filter("../Data/Terrain/");
    Demeter::Settings::GetInstance()->SetMediaPath(mediaPath.c_str());

    Demeter::Settings::GetInstance()->SetVerbose(false);
    Demeter::Settings::GetInstance()->SetScreenWidth(g_ScreenWidth);
    Demeter::Settings::GetInstance()->SetScreenHeight(g_ScreenHeight);
    Demeter::Settings::GetInstance()->SetPreloadTextures(m_bPreloadTextures);
    Demeter::Settings::GetInstance()->SetTextureCompression(m_bTextureCompression);
    Demeter::Settings::GetInstance()->SetUseDynamicTextures(m_bDynamicTextures);
	Demeter::Settings::GetInstance()->SetDetailTextureRepeats(32);
	Demeter::Settings::GetInstance()->SetUseNormals(true);


}

