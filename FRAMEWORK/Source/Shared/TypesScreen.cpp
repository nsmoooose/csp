#include "TypesScreen.h"

StandardScreen::StandardScreen(StandardGraphics *Graphics)
{

  p_3D = Graphics;
  p_Init = p_3D->GetInit();

  p_Back = 0;

  p_WindowCount = 0;
  p_UniqueID = 0;

  p_FocusedWindow = 0;

  p_MouseDown = false;

}

StandardScreen::~StandardScreen()
{
  Uninitialize();
}

short StandardScreen::Initialize(unsigned char UniqueID, StandardColor *ScreenColor)
{

  p_UniqueID = UniqueID;

  if(ScreenColor != 0)
  {    

    p_BackgroundColor = *ScreenColor;

    p_Back = p_3D->CreatePolygonFlat();

    p_Back->Initialize(0,0,1,1, false, &p_BackgroundColor);
  }

  return 0;
}

void StandardScreen::Uninitialize()
{
  if(p_Back != 0)
  {
    p_Back->Uninitialize();
    delete p_Back;
    p_Back=0;
  }

  return;
}

void StandardScreen::Apply()
{

  return;
}

StandardWindow* StandardScreen::CreateGUIWindow(char* WindowName, _Rect *WindowSize)
{

  p_Window[p_WindowCount] = new StandardWindow(p_3D);

  p_Window[p_WindowCount]->Initialize(WindowName, WindowSize);

  p_WindowCount++;

  return p_Window[p_WindowCount - 1];
}

unsigned long StandardScreen::Draw()
{

  unsigned long TPF = 0;

  if(p_Back != 0)
  {
    TPF+= p_Back->Draw();
  }

  for(short x = p_WindowCount - 1;x >= 0;x--)
  {
    TPF += p_Window[x]->Draw();
  }

  return TPF;
}

StandardWindow* StandardScreen::GetWindow(unsigned char WindowNumber)
{
  return p_Window[WindowNumber];
}

unsigned char StandardScreen::GetUniqueID()
{
  return p_UniqueID;
}

_GUI StandardScreen::Process(_GUI_KEY *key, _GUI_MOUSE *mouse)
{

  _GUI GUI;

  GUI.Button = -1;
  GUI.Window = -1;
  GUI.Screen = -1;

  bool b_FocusChange = false;

RETRY:

  for(unsigned char x=0;x < p_WindowCount;x++)
  {
    if(mouse != 0)
    {
      if(p_MouseDown == false && mouse->down == true && b_FocusChange == false)
      {

        _Rect tempRect;

        tempRect = *p_Window[x]->GetCoords();

        // check hit window
        if(mouse->pt.x > tempRect.x1 && mouse->pt.x < tempRect.x2 &&
           mouse->pt.y > tempRect.y1 && mouse->pt.y < tempRect.y2)
        {   
            // make focused
          p_FocusedWindow = p_Window[x];

          // sort window so its on top.
          StandardWindow *tempWindow;

          tempWindow = p_Window[x];

          for(char y=x-1;y>=0;y--)
          {
            p_Window[y + 1] = p_Window[y];
          }

          p_Window[0] = tempWindow;
          b_FocusChange = true;
          goto RETRY;
        }
      }
    }

    if(p_FocusedWindow != 0)
    {
      if(p_FocusedWindow == p_Window[x])
      {
        GUI.Button = p_Window[x]->Process(key, mouse);
      }
      else
      {
        GUI.Button = p_Window[x]->Process(0, 0);
      }
    }
    else
    {
      GUI.Button = p_Window[x]->Process(0, 0);
    }

    if(GUI.Button != -1)
    {
      GUI.Window = x;
      GUI.Screen = p_UniqueID;

      break;
    }
  }

  if(mouse != 0)
  {
    p_MouseDown = mouse->down;
  }

  return GUI;

}
