#include "TypesEdit.h"

StandardEdit::StandardEdit(StandardGraphics *Graphics)
{
  p_3D = Graphics;
  p_Init = p_3D->GetInit();

  p_Color.Initialize(50,50,50,255);

  p_Name = 0;

  p_Window = 0;
  p_WindowOff = 0;
  p_Border = 0;

  p_Down = false;

}

StandardEdit::~StandardEdit()
{
  Uninitialize();
}

short StandardEdit::Initialize(char *Name, _Rect *LabelSize, _Rect *NonScaled)
{

  SetCaption(Name);

  p_NonScaled = *NonScaled;

  Resize(LabelSize);

  return 0;
}

void StandardEdit::Uninitialize()
{

  if(p_Border != 0)
  {
    p_Border->Uninitialize();
    delete p_Border;
    p_Border = 0;
  }

  if(p_WindowOff != 0)
  {
    p_WindowOff->Uninitialize();
    delete p_WindowOff;
    p_WindowOff = 0;
  }

  if(p_Window != 0)
  {
    p_Window->Uninitialize();
    delete p_Window;
    p_Window = 0;
  }

  if(p_Name != 0)
  {
    delete[]p_Name;
    p_Name = 0;
  }

  return;
}

void StandardEdit::Resize(_Rect *LabelSize)
{

  p_Rect = *LabelSize;

  StandardColor color;

  p_Window = p_3D->CreatePolygonFlat();
  p_WindowOff = p_3D->CreatePolygonFlat();
  p_Border = p_3D->CreatePolygonFlat();

  color.Initialize(200,225,255,200);

  p_WindowOff->Initialize(p_Rect.x1,p_Rect.y1,p_Rect.x2,p_Rect.y2, false, &color);

  color.Initialize(255,255,255,200);

  p_Window->Initialize(p_Rect.x1,p_Rect.y1,p_Rect.x2,p_Rect.y2, false, &color);

  StandardTLVertex  Vertexes[8];

  Vertexes[0].Initialize( p_Rect.x1 * p_Init->Width,  p_Rect.y1 * p_Init->Height,0,  0,  0,100);
  Vertexes[1].Initialize( p_Rect.x1 * p_Init->Width,  p_Rect.y1 * p_Init->Height,   0,  0,  0,100);

  Vertexes[2].Initialize( p_Rect.x2 * p_Init->Width,  p_Rect.y1 * p_Init->Height,    0,  0,  0,100);
  Vertexes[3].Initialize( p_Rect.x2 * p_Init->Width,  p_Rect.y1 * p_Init->Height,    0,  0,  0,150);

  Vertexes[4].Initialize( p_Rect.x1 * p_Init->Width,  p_Rect.y2 * p_Init->Height,    0,  0,  0,100);
  Vertexes[5].Initialize( p_Rect.x1 * p_Init->Width,  p_Rect.y2 * p_Init->Height,    0,  0,  0,150);

  Vertexes[6].Initialize( p_Rect.x2 * p_Init->Width,  p_Rect.y2 * p_Init->Height,    0,  0,  0,150);
  Vertexes[7].Initialize( p_Rect.x2 * p_Init->Width,  p_Rect.y2 * p_Init->Height,    0,  0,  0,150);

  p_Border->Initialize(FW_LINES, true,  8, Vertexes);

  short Indx[8];

  Indx[0] = 0;
  Indx[1] = 2;

  Indx[2] = 3;
  Indx[3] = 6;

  Indx[4] = 5;
  Indx[5] = 7;

  Indx[6] = 1;
  Indx[7] = 4;

  p_Border->SetIndexBuffer(Indx, 8);

  return;
}

unsigned long StandardEdit::Draw()
{
  unsigned long TPF = 0;

  if(p_Down == true)
  {
    TPF += p_Window->Draw();
  }
  else
  {
    TPF += p_WindowOff->Draw();
  }
  TPF += p_Border->Draw();

  p_3D->DrawTextF(p_Name, p_Rect, 0, 0, &p_Color);

  return TPF;
}

_Rect*  StandardEdit::GetCoords()
{
  return &p_Rect;
}

_Rect*  StandardEdit::GetNonScaledCoords()
{
  return &p_NonScaled;
}

void StandardEdit::SetCaption(char *Text)
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

char*  StandardEdit::GetCaption()
{
  return p_Name;
}

void StandardEdit::SetDown(bool Down)
{
  p_Down = Down;
}