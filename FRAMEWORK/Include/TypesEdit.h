#ifndef STANDARDEDIT_H_
#define STANDARDEDIT_H_

#include "Framework.h"

class StandardEdit
{
  private:

    StandardGraphics        *p_3D;
    _GRAPHICS_INIT_STRUCT   *p_Init;

    _Rect                    p_Rect;
    _Rect                    p_NonScaled;

    StandardColor            p_Color;
    char                    *p_Name;

    StandardPolygonFlat     *p_Window;
    StandardPolygonFlat     *p_WindowOff;

    StandardPolygonFlat     *p_Border;

    bool                     p_Down;

  public:

    StandardEdit(StandardGraphics *Graphics);
    ~StandardEdit();

    short           Initialize(char *Name, _Rect *LabelSize, _Rect *NonScaled);
    void            Uninitialize();

    unsigned long   Draw();
    _Rect*          GetCoords();
    _Rect*          GetNonScaledCoords();

    void            Resize(_Rect *LabelSize);
    void            SetCaption(char *Text);
    char*           GetCaption();

    void            SetDown(bool Down);

};

#endif
