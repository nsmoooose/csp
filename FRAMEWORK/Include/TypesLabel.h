#ifndef STANDARDLABEL_H_
#define STANDARDLABEL_H_

#include "Framework.h"

class StandardLabel
{
  private:

    StandardGraphics        *p_3D;
    _GRAPHICS_INIT_STRUCT   *p_Init;

    _Rect                    p_Rect;
    _Rect                    p_NonScaled;

    StandardColor            p_Color;
    char                    *p_Name;

  public:

    StandardLabel(StandardGraphics *Graphics);
    ~StandardLabel();

    short           Initialize(char *Name, _Rect *LabelSize, _Rect *NonScaled);
    void            Uninitialize();

    unsigned long   Draw();
    _Rect*          GetCoords();
    _Rect*          GetNonScaledCoords();

    void            Resize(_Rect *LabelSize);
    void            SetCaption(char *Text);
    char*           GetCaption();

};

#endif
