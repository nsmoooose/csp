#include "stdinc.h"


#include "TerrainSymbol.h"
#include "global.h"

TerrainSymbol::TerrainSymbol()
{
	m_pTerrainObject = new TerrainObject;
	
}

TerrainSymbol::~TerrainSymbol()
{
	delete m_pTerrainObject;
}

std::string TerrainSymbol::SetCommand(std::deque<std::string> & args)
{
  	std::string propString = args[0];
        ConvertStringToUpper(propString);
	// SET terrain ACTIVE TRUE|FALSE
	if (propString == "ACTIVE")
	{
		std::string valueString = args[1];
        	ConvertStringToUpper(valueString);
		if (valueString == "TRUE")
			m_pTerrainObject->SetActive(true);
		else
			m_pTerrainObject->SetActive(false);
		return "OK";
	}
	// SET terrain LATTICE TRUE|FALSE
	else if (propString == "LATTICE")
	{
		std::string valueString = args[1];
        	ConvertStringToUpper(valueString);
		bool value = ( (valueString == "TRUE") ? true : false );
		m_pTerrainObject->m_bLattice = value;
		return "OK";
	}
	// SET terrain VERTEXSPACING value
	else if (propString == "VERTEXSPACING")
	{
		std::string valueString = args[1];
		float value = atof(valueString.c_str() );
		m_pTerrainObject->m_fVertexSpacing = value;
		return "OK";
	}
	// SET terrain VERTEXHEIGHT value
	else if (propString == "VERTEXHEIGHT")
	{
		std::string valueString = args[1];
		float value = atof(valueString.c_str() );
		m_pTerrainObject->m_fVertexHeight = value;
		return "OK";
	}
	// SET terrain ELEVFILE filename
	else if (propString == "ELEVFILE")
	{
		std::string valueString = args[1];
		m_pTerrainObject->m_sElevationFile = valueString;
		return "OK";
	}
	// SET terrain TEXTUREFILE filename
	else if (propString == "TEXTUREFILE")
	{
		std::string valueString = args[1];
		m_pTerrainObject->m_sTextureFile = valueString;
		return "OK";
	}
	// SET terrain DETAILTEXTUREFILE filename
	else if (propString == "DETAILTEXTUREFILE")
	{
		std::string valueString = args[1];
		m_pTerrainObject->m_sDetailTextureFile = valueString;
		return "OK";
	}
	// SET terrain MAXTRIANGLES value
	else if (propString == "MAXTRIANGLES")
	{
		std::string valueString = args[1];
		int value = atoi(valueString.c_str() );
		m_pTerrainObject->m_iMaxTriangles = value;
		return "OK";
	}
	// SET terrain PRELOADTEXTURES TRUE|FALSE
	else if (propString == "PRELOADTEXTURES")
	{
		std::string valueString = args[1];
        	ConvertStringToUpper(valueString);
		bool value = ( (valueString == "TRUE") ? true : false );
		m_pTerrainObject->m_bPreloadTextures = value;
		return "OK";
	}
	// SET terrain TEXTURECOMPRESSION TRUE|FALSE
	else if (propString == "TEXTURECOMPRESSION")
	{
		std::string valueString = args[1];
        	ConvertStringToUpper(valueString);
		bool value = ( (valueString == "TRUE") ? true : false );
		m_pTerrainObject->m_bTextureCompression = value;
		return "OK";
	}
	// Set terrain DYNAMICTEXTURES TRUE|FALSE
	else if (propString == "DYNAMICTEXTURES")
	{
		std::string valueString = args[1];
        	ConvertStringToUpper(valueString);
		bool value = ( (valueString == "TRUE") ? true : false );
		m_pTerrainObject->m_bDynamicTextures = value;
		return "OK";
	}
	// SET terrain DETAILTHRESHOLD value
	else if (propString == "DETAILTHRESHOLD")
	{
		std::string valueString = args[1];
		float value = atof(valueString.c_str() );
		m_pTerrainObject->m_fDetailThreshold = value;
		return "OK";
	}
	// SET terrain LATTICEBASENAME value
	else if (propString == "LATTICEBASENAME")
	{
		std::string valueString = args[1];
		m_pTerrainObject->m_sLatticeBaseName = valueString;		
	}
	else if (propString == "LATTICEELEVEXT")
	{
		std::string valueString = args[1];
		m_pTerrainObject->m_sLatticeElevExt = valueString;
	}
	else if (propString == "LATTICETEXEXT")
	{
		std::string valueString = args[1];
		m_pTerrainObject->m_sLatticeTexExt = valueString;
	}
	else if (propString == "LATTICEWIDTH")
	{
		std::string valueString = args[1];
		int value = atoi(valueString.c_str());
		m_pTerrainObject->m_sLatticeWidth = value;
	}
	else if (propString == "LATTICEHEIGHT")
	{
		std::string valueString = args[1];
		int value = atoi(valueString.c_str());
		m_pTerrainObject->m_sLatticeHeight = value;
	}
	else if (propString == "TEXTUREFACTORY")
	{
		std::string valueString = args[1];
        	ConvertStringToUpper(valueString);
		bool value = ( (valueString == "TRUE") ? true : false );
		m_pTerrainObject->m_bTextureFactory = value;
		return "OK";
	}

	return "Not yet implemented:";
}

std::string TerrainSymbol::GetCommand(std::deque<std::string> & args)
{
	return "Not yet implemented:";
}


std::ostream & operator << (std::ostream & os, const TerrainSymbol& sym)
{
	os << "SymbolClass: " << typeid(sym).name();
	return os;

}

