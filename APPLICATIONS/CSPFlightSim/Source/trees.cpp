#include <fstream>
#include <sstream>

#include <osg/Billboard>
#include <osg/BlendFunc>
#include <osg/Texture2D>

#ifdef _WIN32
	#include <osgDB/FileUtils>
#endif

#include <osgDB/ReadFile>


#include "Trees.h"

#ifdef _MSC_VER
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

extern double g_LatticeXDist;
extern double g_LatticeYDist;

using namespace osg;

TreesFactory::TreesFactory()
{	
	std::vector<std::string> treefileName;
	
	std::string dirBase("../Data/Images/");

	// read tree textures from file
	ifstream iStream("../Data/Scripts/Trees.csp");
    std::string str;
	while (iStream >> str)
	  treefileName.push_back(dirBase + str);

	iStream.close();

	m_treesNumber = treefileName.size();

    // we don't need this on Linux since libs are usually
	// installed in /usr/local/lib/osgPlugins or /usr/lib/osgPlugins.
	// OSG can find itself the plugins.
#ifdef _WIN32
	osgDB::setLibraryFilePathList("../Bin");
#endif

	_tree aTree;
    for (unsigned short i=0;i<m_treesNumber;++i)
	{
		// makes a generic tree geometry
		aTree.h = 7.0 + 3.0 * i / m_treesNumber;
		aTree.w = aTree.h ;
		m_treesArray.push_back(MakeAGeomTree(aTree));

        // picks a texture
		Texture2D * aTex = new Texture2D;
		osg::Image * image = osgDB::readImageFile ( treefileName[i] );
		aTex->setImage(image);

		// makes a generic state
		ref_ptr<StateSet> dstate = new StateSet;
		dstate->setTextureAttributeAndModes(0, aTex, StateAttribute::ON );	
		dstate->setTextureAttribute(0, new TexEnv );
		dstate->setAttributeAndModes( new BlendFunc, StateAttribute::ON );
		AlphaFunc* alphaFunc = new AlphaFunc;
		alphaFunc->setFunction(AlphaFunc::GEQUAL,0.05f);
		dstate->setAttributeAndModes( alphaFunc, StateAttribute::ON );		
		dstate->setMode( GL_LIGHTING, StateAttribute::OFF );		
		dstate->setRenderingHint( StateSet::TRANSPARENT_BIN );				
		m_statesArray.push_back(dstate);
	}
}

Geometry const * TreesFactory::GetTree() const
{
	return m_treesArray[static_cast<size_t>(UnsignedRandomNumber() * m_treesNumber)].get();
}

StateSet const * TreesFactory::GetState() const
{
  return m_statesArray[static_cast<size_t>(UnsignedRandomNumber() * m_treesNumber)].get();
}

StateSet const * TreesFactory::GetState(size_t const indice) const
{
  return m_statesArray[indice].get();
}

size_t TreesFactory::GetTreesNumber() const
{
	return m_treesNumber;
}


Geometry * TreesFactory::MakeAGeomTree( _tree const &p_tree) const
{
    float vv[][3] =
    {
        { -p_tree.w/2.0, 0.0, 0.0 },
        {  p_tree.w/2.0,  0.0, 0.0 },
        {  p_tree.w/2.0, 0.0, 2.0 * p_tree.h },
        { -p_tree.w/2.0, 0.0, 2.0 * p_tree.h },
    };

    Vec3Array& v = *(new Vec3Array(4));
    Vec2Array& t = *(new Vec2Array(4));

    for(unsigned short i = 0; i < 4; i++ )
    {
        v[i][0] = vv[i][0];
        v[i][1] = vv[i][1];
        v[i][2] = vv[i][2];
    }

    t[0][0] = 0.0; t[0][1] = 0.0;
    t[1][0] = 1.0; t[1][1] = 0.0;
    t[2][0] = 1.0; t[2][1] = 1.0;
    t[3][0] = 0.0; t[3][1] = 1.0;

    Geometry * geom = new Geometry;

    geom->setVertexArray( &v );

    geom->setTexCoordArray( 0, &t );

    geom->setColorBinding( Geometry::BIND_OVERALL );

    geom->addPrimitiveSet( new DrawArrays(PrimitiveSet::QUADS,0,4) );
    return geom;
}

TreesFactory const Forest::m_treesFactory;

float const Forest::sizePatch = 128.0;


Group * Forest::MakePatchForest(Vec2 const & p_center, float const p_length) const
{
	const unsigned short treePerPatchNumber = 2;	
	
	Group * patchForest = new Group;
	
	float baseRadius = p_length / 4.0;
	
	unsigned long  count = 0;
	for (unsigned short i = 0; i < treePerPatchNumber;++i)
	{
		//abTree->setMode(osg::Billboard::POINT_ROT_EYE );
		float angle = 2.0 * PI * UnsignedRandomNumber();
		float spacing = (1.0 + UnsignedRandomNumber()) * baseRadius;
		float x = p_center.x() + spacing * cos(angle);
		float y = p_center.y() + spacing * sin(angle);
		if ( Predicate(Vec2(x,y)) )
		{
			float z = m_virtualBattlefield->getElevation(x, y);
			x = fmod(x, g_LatticeXDist);
			y = fmod(y, g_LatticeYDist);
			Vec3 pos(x,y,z);
			Geometry * agTree = new Geometry(*m_treesFactory.GetTree());
			Billboard *abTree = new Billboard;
			abTree->addDrawable( agTree, pos );
			patchForest->addChild( abTree);
			++count;
		}
	}
	if (count == 0)
		return 0;
	return patchForest;
}

float Forest::f(float x, float y) const
{
	return x*x+y*y-1.0;
}

float Forest::g(float x, float y) const
{
  float r = m_randomParam;
  return (x-1.0)*(x-1.0)+y*y-r*r/4.0; 
}

float Forest::HeightField(Vec2 const & p_p) const
{ // f:D'(0,1)->R 
 Vec3 p = m_randomRotation * Vec3(p_p.x(), p_p.y(),0);

 float x = p.x();
 float y = p.y();
 float a = f(x,y);
 float b = g(x,y);
 float h = exp(-a*a)*a+exp(-b*b)*b;
 return h;
}

bool Forest::Predicate(Vec2 const & p_p) const
{
	if (HeightField((p_p - m_center)*2.0/m_length) < 0)
		return true;
	else
		return false;
}


Group * Forest::MakeForest(Vec2 const p_center, float const p_length) const
{
	if ( p_length < sizePatch )
	{
		Group * aForestPatch = MakePatchForest(p_center, p_length);
		if (aForestPatch)
		{
			aForestPatch->setStateSet(m_state);
			return aForestPatch;
		}
		else
			return 0;
	}
	else
	{   
		float length_2 = p_length / 2.0;
		float length_4 = length_2 / 2.0;
		Vec2 NEOffset = Vec2(length_4, length_4);
		Vec2 NWOffset = Vec2(-length_4, length_4);
		
		osg::Group * NWForest = MakeForest(p_center + NWOffset, length_2);
		osg::Group * NEForest = MakeForest(p_center + NEOffset, length_2);
		osg::Group * SWForest = MakeForest(p_center - NEOffset, length_2);
		osg::Group * SEForest = MakeForest(p_center - NWOffset, length_2);
		
		osg::Group * forest = new osg::Group;
        bool empty = true;
		
		if (NWForest)
		{
			empty = false;
			forest->addChild(NWForest);
		}
		if (NEForest)
		{
			empty = false;
			forest->addChild(NEForest);
		}
		if (SWForest)
		{
			empty = false;
			forest->addChild(SWForest);
		}
		if (SEForest)
		{
			empty = false;
			forest->addChild(SEForest);
		}
        if (empty)
			return 0;
		else
			return forest;
	}
}

Forest::Forest(Vec2 const p_center, float const p_length, VirtualBattlefield * pBattlefield, short const p_state)
{
	m_virtualBattlefield = pBattlefield;
	m_randomParam = SignedRandomNumber();
	m_center = p_center;
	m_length = p_length;
	m_randomRotation.makeRotate(m_randomParam * PI, Z_AXIS); 
	unsigned short iState;
	if ( p_state == -1 )
		iState = static_cast<unsigned short>(UnsignedRandomNumber() * m_treesFactory.GetTreesNumber());
	else
		iState = p_state % m_treesFactory.GetTreesNumber();	
	m_state = new StateSet(*m_treesFactory.GetState(iState));
	addChild(MakeForest(p_center,p_length));
}