#include <osg/Depth>
#include <osgText/Text>
#include <osg/Material>
#include <osg/Geode>
#include <osg/Group>

#include "stdinc.h"
#include "BaseObject.h"
#include "CockpitDrawable.h"



extern int g_ScreenWidth;
extern int g_ScreenHeight;

#define        TEXT_BITMAP        "Bitmap Font - jygq"
osgText::Text::AlignmentType    gAlignment=osgText::Text::LEFT_BOTTOM;
int    gFontSize=18;
#define        TEXT_COL_2D        osg::Vec4(.9,.9,.9,1)
#define        TEXT_COL_3D        osg::Vec4(.99,.3,.2,1)


CockpitDrawable::CockpitDrawable()
{
	/*setSupportsDisplayList(false);

    std::string    ttfPath("fonts/times.ttf");
    osg::Geode*        geode;
    osg::Material*    textMaterial;
    osg::StateSet*  textState;
    double            xOffset = 250;
    double            yOffset = gFontSize+10;

    //rootNode = new osg::Group();

    int    gFontSize1 = 24;
    
    osgText::BitmapFont*    bitmapFont = osgNew osgText::BitmapFont(ttfPath,
                                                               gFontSize1);
	osgText:: Text * m_text = osgNew osgText::Text(bitmapFont);
    
    std::vector<osg::ref_ptr<osgText::Text > >    gTextList;
    gTextList.push_back(m_text);


    m_text->setText(std::string("2d ") + std::string(TEXT_BITMAP));
  
    m_text->setDrawMode( osgText::Text::TEXT |
                       osgText::Text::BOUNDINGBOX |
                       osgText::Text::ALIGNMENT );
    m_text->setAlignment(gAlignment);
	m_text->setColor(TEXT_COL_2D);

    geode = osgNew osg::Geode();
    geode->setName("BitmapFont");
    geode->addDrawable( m_text );

    //textMaterial = new osg::Material();
    //textMaterial->setColorMode( osg::Material::AMBIENT_AND_DIFFUSE);
    //textMaterial->setDiffuse( osg::Material::FRONT_AND_BACK, TEXT_COL_2D);
    //textState = new osg::StateSet();
    //textState->setAttribute(textMaterial );
    //geode->setStateSet( textState );


	osg::Depth* depth = osgNew osg::Depth;
	depth->setRange(0.0,0.0);

	osg::StateSet* rootState = osgNew osg::StateSet();
	rootState->setAttribute(depth);

    */


}

CockpitDrawable::CockpitDrawable(const CockpitDrawable& other,const osg::CopyOp& copyop) :
        Drawable(other,copyop)
{

}


CockpitDrawable::~CockpitDrawable()
{

}

osg::Object* CockpitDrawable::cloneType() const
{
    // return a clone of this type of drawable.
	return new CockpitDrawable();

}

osg::Object* CockpitDrawable::clone(const osg::CopyOp& copyop) const
{
    // return a clone of this object, via the copy constructor.
    return new CockpitDrawable(*this,copyop);

}

CockpitDrawable&  CockpitDrawable::operator = (const CockpitDrawable&)
{
	return *this;

}

const char*  CockpitDrawable::className() const
{
	return "CockpitDrawable"; 
}


bool CockpitDrawable::computeBound() const
{
    return false;
}



void CockpitDrawable::drawImplementation(osg::State& state) const
{

	/*
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0 , g_ScreenWidth, 0 , g_ScreenHeight, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

    glColor3f(0.5, 0.5, 0.5);

	int halfHeight = g_ScreenHeight >> 1;
	int halfWidth = g_ScreenWidth >> 1;

    glBegin(GL_LINES);
    glVertex2f(halfWidth-10, halfHeight);
    glVertex2f(halfWidth+10, halfHeight);
    glVertex2f(halfWidth, halfHeight-10);
    glVertex2f(halfWidth, halfHeight+10);
    glEnd();

  	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
    */
	
}
