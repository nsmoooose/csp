#include "TypesButton.h"

StandardButton::StandardButton(StandardGraphics *Graphics)
{
  p_3D = Graphics;
  p_Init = p_3D->GetInit();

  p_Color.Initialize(50,50,50,255);

  p_UniqueID = 0;

  p_Name = 0;
  p_BorderOut = 0;
  p_BorderUp = 0;
  p_BorderDown = 0;
  p_Window = 0;

  p_Down = false;

}

StandardButton::~StandardButton()
{
  Uninitialize();
}

short StandardButton::Initialize(unsigned char UniqueID, char *Name, _Rect *ButtonSize, _Rect *NonScaled)
{

  p_UniqueID = UniqueID;

  p_Name = new char[strlen(Name) + 1];

  memset(p_Name, 0, strlen(Name) + 1);

  strcpy(p_Name, Name);

  p_NonScaled = *NonScaled;

  Resize(ButtonSize);

  return 0;
}

void StandardButton::Uninitialize()
{

  if(p_Window != 0)
  {
    p_Window->Uninitialize();
    delete p_Window;
    p_Window = 0;
  }

  if(p_BorderUp != 0)
  {
    p_BorderUp->Uninitialize();
    delete p_BorderUp;
    p_BorderUp = 0;
  }

  if(p_BorderDown != 0)
  {
    p_BorderDown->Uninitialize();
    delete p_BorderDown;
    p_BorderDown = 0;
  }

  if(p_BorderOut != 0)
  {
    p_BorderOut->Uninitialize();
    delete p_BorderOut;
    p_BorderOut = 0;
  }

  if(p_Name != 0)
  {
    delete[]p_Name;
    p_Name = 0;
  }

  return;
}

void StandardButton::Resize(_Rect *ButtonSize)
{

  if(p_Window != 0)
  {
    p_Window->Uninitialize();
    delete p_Window;
    p_Window = 0;
  }

  if(p_BorderUp != 0)
  {
    p_BorderUp->Uninitialize();
    delete p_BorderUp;
    p_BorderUp = 0;
  }

  if(p_BorderDown != 0)
  {
    p_BorderDown->Uninitialize();
    delete p_BorderDown;
    p_BorderDown = 0;
  }

  if(p_BorderOut != 0)
  {
    p_BorderOut->Uninitialize();
    delete p_BorderOut;
    p_BorderOut = 0;
  }

  p_Rect = *ButtonSize;

  p_Window = p_3D->CreatePolygonFlat();
  p_BorderOut = p_3D->CreatePolygonFlat();
  p_BorderUp = p_3D->CreatePolygonFlat();
  p_BorderDown = p_3D->CreatePolygonFlat();

  StandardColor color;

  color.Initialize(200,225,255,150);

  p_Window->Initialize(ButtonSize->x1,ButtonSize->y1,ButtonSize->x2,ButtonSize->y2, false, &color);

  StandardTLVertex  Vertexes[8];

  Vertexes[0].Initialize( ButtonSize->x1 * p_Init->Width,  ButtonSize->y1 * p_Init->Height,0,  0,  0,100);
  Vertexes[1].Initialize( ButtonSize->x1 * p_Init->Width,  ButtonSize->y1 * p_Init->Height,   0,  0,  0,100);

  Vertexes[2].Initialize( ButtonSize->x2 * p_Init->Width,  ButtonSize->y1 * p_Init->Height,    0,  0,  0,100);
  Vertexes[3].Initialize( ButtonSize->x2 * p_Init->Width,  ButtonSize->y1 * p_Init->Height,    0,  0,  0,150);

  Vertexes[4].Initialize( ButtonSize->x1 * p_Init->Width,  ButtonSize->y2 * p_Init->Height,    0,  0,  0,100);
  Vertexes[5].Initialize( ButtonSize->x1 * p_Init->Width,  ButtonSize->y2 * p_Init->Height,    0,  0,  0,150);

  Vertexes[6].Initialize( ButtonSize->x2 * p_Init->Width,  ButtonSize->y2 * p_Init->Height,    0,  0,  0,150);
  Vertexes[7].Initialize( ButtonSize->x2 * p_Init->Width,  ButtonSize->y2 * p_Init->Height,    0,  0,  0,150);

  p_BorderOut->Initialize(FW_LINES, true,  8, Vertexes);

  short Indx[8];

  Indx[0] = 0;
  Indx[1] = 2;

  Indx[2] = 3;
  Indx[3] = 6;

  Indx[4] = 5;
  Indx[5] = 7;

  Indx[6] = 1;
  Indx[7] = 4;

  p_BorderOut->SetIndexBuffer(Indx, 8);

  // ---
  Vertexes[0].Initialize( ButtonSize->x1 * p_Init->Width + 1,  ButtonSize->y1 * p_Init->Height + 1,         255,  255,  255, 200);
  Vertexes[1].Initialize( ButtonSize->x1 * p_Init->Width + 1,  ButtonSize->y1 * p_Init->Height + 1,     255,  255,  255, 200);

  Vertexes[2].Initialize( ButtonSize->x2 * p_Init->Width - 1,  ButtonSize->y1 * p_Init->Height + 1,    255,  255,  255, 200);
  Vertexes[3].Initialize( ButtonSize->x2 * p_Init->Width - 1,  ButtonSize->y1 * p_Init->Height + 1,    0,  0,  0, 50);

  Vertexes[4].Initialize( ButtonSize->x1 * p_Init->Width + 1,  ButtonSize->y2 * p_Init->Height - 1,    255,  255,  255, 200);
  Vertexes[5].Initialize( ButtonSize->x1 * p_Init->Width + 1,  ButtonSize->y2 * p_Init->Height - 1,    0,  0,  0, 50);

  Vertexes[6].Initialize( ButtonSize->x2 * p_Init->Width - 1,  ButtonSize->y2 * p_Init->Height - 1,    0,  0,  0, 50);
  Vertexes[7].Initialize( ButtonSize->x2 * p_Init->Width - 1,  ButtonSize->y2 * p_Init->Height - 1,    0,  0,  0, 50);

  p_BorderUp->Initialize(FW_LINES, true,  8, Vertexes);

  // --

  Indx[0] = 0;
  Indx[1] = 2;

  Indx[2] = 3;
  Indx[3] = 6;

  Indx[4] = 5;
  Indx[5] = 7;

  Indx[6] = 1;
  Indx[7] = 4;

  p_BorderUp->SetIndexBuffer(Indx, 8);

  // ---
  Vertexes[0].Initialize( ButtonSize->x1 * p_Init->Width,  ButtonSize->y1 * p_Init->Height + 1,         255,  255,  255, 50);
  Vertexes[1].Initialize( ButtonSize->x1 * p_Init->Width + 1,  ButtonSize->y1 * p_Init->Height + 1,     255,  255,  255, 50);

  Vertexes[2].Initialize( ButtonSize->x2 * p_Init->Width - 1,  ButtonSize->y1 * p_Init->Height + 1,    255,  255,  255, 50);
  Vertexes[3].Initialize( ButtonSize->x2 * p_Init->Width - 1,  ButtonSize->y1 * p_Init->Height + 1,    0,  0,  0, 12);

  Vertexes[4].Initialize( ButtonSize->x1 * p_Init->Width + 1,  ButtonSize->y2 * p_Init->Height - 1,    255,  255,  255, 50);
  Vertexes[5].Initialize( ButtonSize->x1 * p_Init->Width + 1,  ButtonSize->y2 * p_Init->Height - 1,    0,  0,  0, 12);

  Vertexes[6].Initialize( ButtonSize->x2 * p_Init->Width - 1,  ButtonSize->y2 * p_Init->Height - 1,    0,  0,  0, 12);
  Vertexes[7].Initialize( ButtonSize->x2 * p_Init->Width - 1,  ButtonSize->y2 * p_Init->Height - 1,    0,  0,  0, 12);

  p_BorderDown->Initialize(FW_LINES, true,  8, Vertexes);

  // --

  Indx[0] = 0;
  Indx[1] = 2;

  Indx[2] = 3;
  Indx[3] = 6;

  Indx[4] = 5;
  Indx[5] = 7;

  Indx[6] = 1;
  Indx[7] = 4;

  p_BorderDown->SetIndexBuffer(Indx, 8);

  return;
}

unsigned long StandardButton::Draw()
{
  unsigned long TPF = 0;

  TPF += p_Window->Draw();
  TPF += p_BorderOut->Draw();

  if (p_Down == false)
  {
    TPF += p_BorderUp->Draw();
  }
  else
  {
    TPF += p_BorderDown->Draw();
  }

  p_3D->DrawTextF(p_Name, p_Rect, 1, 1, &p_Color);

  return TPF;
}

_Rect*  StandardButton::GetCoords()
{
  return &p_Rect;
}

_Rect*  StandardButton::GetNonScaledCoords()
{
  return &p_NonScaled;
}

unsigned char StandardButton::GetUniqueID()
{
  return p_UniqueID;
}

void StandardButton::SetDown(bool Down)
{
  p_Down = Down;
}