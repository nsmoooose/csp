#ifndef STANDARDBUTTON_H_
#define STANDARDBUTTON_H_

#include "Framework.h"

class StandardButton
{
  private:

    StandardGraphics        *p_3D;
    _GRAPHICS_INIT_STRUCT   *p_Init;

    _Rect                    p_Rect;
    _Rect                    p_NonScaled;

    StandardColor            p_Color;
    char                    *p_Name;
    unsigned char            p_UniqueID;

    StandardPolygonFlat     *p_Window;

    StandardPolygonFlat     *p_BorderOut;
    StandardPolygonFlat     *p_BorderUp;
    StandardPolygonFlat     *p_BorderDown;
    bool                     p_Down;

  public:

    StandardButton(StandardGraphics *Graphics);
    ~StandardButton();

    short           Initialize(unsigned char UniqueID, char *Name, _Rect *ButtonSize, _Rect *NonScaled);
    void            Uninitialize();

    unsigned long   Draw();
    _Rect*          GetCoords();
    _Rect*          GetNonScaledCoords();
    unsigned char   GetUniqueID();

    void            SetDown(bool Down);
    void            Resize(_Rect *ButtonSize);

};

#endif
