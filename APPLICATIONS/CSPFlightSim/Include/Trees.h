#ifndef __TREES_H__
#define __TREES_H__

#include <osg/Geometry>

#include "VirtualBattlefield.h"

class TreesFactory
{
	struct _tree
	{
		float h,w;
	};
	size_t m_treesNumber;               
	std::vector<osg::ref_ptr<osg::Geometry> > m_treesArray;        
	std::vector<osg::ref_ptr<osg::StateSet> > m_statesArray;
	
	osg::Geometry * MakeAGeomTree( _tree const &p_tree) const;
public:
	osg::Geometry const * GetTree() const;
	osg::StateSet const * GetState() const;
	osg::StateSet const * GetState(size_t const indice) const;
    size_t GetTreesNumber() const;
	TreesFactory();
};


class Forest:public osg::Group
{
	VirtualBattlefield * m_virtualBattlefield;
	float static const sizePatch;
	TreesFactory static const m_treesFactory;
	osg::Vec2 m_center;
	float m_length;
	float m_randomParam;
	osg::Matrix m_randomRotation;
	osg::StateSet * m_state;
	float f(float x, float y) const;
    float g(float x, float y) const;
	float HeightField (osg::Vec2 const & p_p) const;
	bool Predicate (osg::Vec2 const & p_p) const;
	osg::Group * MakePatchForest(osg::Vec2 const & p_center, float const p_length) const;
	osg::Group * MakeForest(osg::Vec2 const p_center, float const p_length) const;
public:
	Forest(osg::Vec2 const p_center, float const length, VirtualBattlefield * pBattlefield, short const p_state = -1);
};

#endif // __TREES_H__
