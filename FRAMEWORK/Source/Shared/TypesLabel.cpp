#include "TypesLabel.h"

StandardLabel::StandardLabel(StandardGraphics *Graphics)
{
  p_3D = Graphics;
  p_Init = p_3D->GetInit();

  p_Color.Initialize(50,50,50,255);

  p_Name = 0;

}

StandardLabel::~StandardLabel()
{
  Uninitialize();
}

short StandardLabel::Initialize(char *Name, _Rect *LabelSize, _Rect *NonScaled)
{

  SetCaption(Name);

  p_NonScaled = *NonScaled;

  Resize(LabelSize);

  return 0;
}

void StandardLabel::Uninitialize()
{

  if(p_Name != 0)
  {
    delete[]p_Name;
    p_Name = 0;
  }

  return;
}

void StandardLabel::Resize(_Rect *LabelSize)
{

  p_Rect = *LabelSize;

  return;
}

unsigned long StandardLabel::Draw()
{
  unsigned long TPF = 0;

  p_3D->DrawTextF(p_Name, p_Rect, 0, 0, &p_Color);

  return TPF;
}

_Rect*  StandardLabel::GetCoords()
{
  return &p_Rect;
}

_Rect*  StandardLabel::GetNonScaledCoords()
{
  return &p_NonScaled;
}

void StandardLabel::SetCaption(char *Text)
{

  if(p_Name != 0)
  {
    delete[]p_Name;
    p_Name = 0;
  }

  p_Name = new char[strlen(Text) + 1];

  memset(p_Name, 0, strlen(Text) + 1);

  strcpy(p_Name, Text);

}

char*  StandardLabel::GetCaption()
{
  return p_Name;
}
