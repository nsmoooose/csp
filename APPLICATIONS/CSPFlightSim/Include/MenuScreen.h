#ifndef __MENUSCREEN_H__
#define __MENUSCREEN_H__

#include <osgText/Text>
#include <osg/Group>

#include "BaseScreen.h"

/**
 * class MenuScreen
 *
 * @author unknown
 */
class MenuScreen : public BaseScreen
{
public:
    virtual void OnInit();
    virtual void OnExit();

    virtual void OnRender();

    virtual bool OnKeyDown(char * key);
    virtual void OnKeyUp(char * key);
    virtual void OnMouseMove(int x, int y);
    virtual void OnMouseButtonDown(int num, int x, int y);
    virtual void OnJoystickAxisMotion(int joynum, int axis, int val);
    virtual void OnJoystickButtonDown(int joynum, int butnum);

private:
        osgText::Text*    m_text;
        osg::Group* rootNode;


};

#endif
