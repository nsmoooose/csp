#include "stdinc.h"

#include <osg/Node>
#include <osg/StateSet>
#include <osg/GeoSet>
#include <osg/Material>
#include <osg/Transparency>
#include <osg/Transform>
#include <osg/PolygonMode>
#include <osg/Depth>
#include <osg/Notify>
#include <osg/Geode>

#include <osgUtil/TrackballManipulator>
#include <osgUtil/FlightManipulator>
#include <osgUtil/DriveManipulator>


//#include <osgText/Text>

//#include <osgText/Font>

#include "MenuScreen.h"


//#define        TEXT_BITMAP        "Bitmap Font - jygq"
//osgText::Text::AlignmentType    gAlignment=osgText::Text::LEFT_BOTTOM;
//int    gFontSize=18;
//#define        TEXT_COL_2D        osg::Vec4(.9,.9,.9,1)
//#define        TEXT_COL_3D        osg::Vec4(.99,.3,.2,1)


void MenuScreen::OnInit()
{
    /*
    std::string    ttfPath("fonts/times.ttf");
    osgText::Text*    text;
    osg::Geode*        geode;
    osg::Material*    textMaterial;
    osg::StateSet*  textState;
    double            xOffset=250;
    double            yOffset=gFontSize+10;

    rootNode = new osg::Group();

    int    gFontSize1=24;
    
    osgText::BitmapFont*    bitmapFont=new osgText::BitmapFont(ttfPath,
                                                               gFontSize1);
    m_text=new osgText::Text(bitmapFont);
    
    std::vector<osg::ref_ptr<osgText::Text > >    gTextList;
    gTextList.push_back(m_text);
    m_text->setText(std::string("2d ")+std::string(TEXT_BITMAP));
    m_text->setPosition(osg::Vec3(0,0,0));
    m_text->setDrawMode( osgText::Text::TEXT |
                       osgText::Text::BOUNDINGBOX |
                       osgText::Text::ALIGNEMENT );
    m_text->setAlignment(gAlignment);

    geode = new osg::Geode();
    geode->setName("BitmapFont");
    geode->addDrawable( text );

    textMaterial = new osg::Material();
    textMaterial->setColorMode( osg::Material::AMBIENT_AND_DIFFUSE);
    textMaterial->setDiffuse( osg::Material::FRONT_AND_BACK, TEXT_COL_2D);
    textState = new osg::StateSet();
    textState->setAttribute(textMaterial );
    geode->setStateSet( textState );

    rootNode->addChild(geode);
    */

}

void MenuScreen::OnExit()
{

}

void MenuScreen::OnRender()
{

}

bool MenuScreen::OnKeyDown(char * key)
{
    return false;
}

void MenuScreen::OnKeyUp(char * key)
{

}
 
void MenuScreen::OnMouseMove(int x, int y)
{

}
 
void MenuScreen::OnMouseButtonDown(int num, int x, int y)
{

}

void MenuScreen::OnJoystickAxisMotion(int joynum, int axis, int val)
{

}

void MenuScreen::OnJoystickButtonDown(int joynum, int butnum)
{

}
