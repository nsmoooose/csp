#ifndef STANDARDWINDOW_H_
#define STANDARDWINDOW_H_

#include "Framework.h"

class StandardWindow
{
  private:

    StandardGraphics        *p_3D;
    _GRAPHICS_INIT_STRUCT   *p_Init;

    StandardPolygonFlat     *p_Window;
    StandardPolygonFlat     *p_ClientArea;
    StandardPolygonFlat     *p_Border;

    StandardButton          *p_Button[50];
    StandardLabel           *p_Label[50];
    StandardEdit            *p_Text[50];
    StandardEdit            *p_FocusedText;

    unsigned char            p_ButtonCount;
    unsigned char            p_LabelCount;
    unsigned char            p_TextCount;

    char                    *p_Name;
    _Rect                    p_Rect;
    _Rect                    p_ClientAreaRect;
    _Rect                    p_TitleRect;
    _Rect                    p_ResizeRect;

    StandardColor            p_Color;
    bool                     p_MouseDown;
    bool                     p_ResizeDown;
    bool                     p_TitleDown;

    _Point                   p_OffSet;

  public:

    StandardWindow(StandardGraphics *Graphics);
    ~StandardWindow();

    short           Initialize(char *Name, _Rect *WindowSize);
    void            Uninitialize();

    unsigned long   Draw();

    StandardButton* CreateButton(unsigned char ButtonNumber, char *Name,  _Rect *Rect);
    StandardLabel*  CreateLabel(char *Name,  _Rect *Rect);
    StandardLabel*  CreateText(char *Name,  _Rect *Rect);

    _Rect*          GetCoords();

    unsigned char   Process(_GUI_KEY *key, _GUI_MOUSE *mouse);
    void            Resize(_Rect *WindowSize);

};

#endif

