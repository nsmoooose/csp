#include "TypesWindow.h"

StandardWindow::StandardWindow(StandardGraphics *Graphics)
{

  p_3D = Graphics;
  p_Init = p_3D->GetInit();


  p_Window = 0;
  p_Border = 0;
  p_ClientArea = 0;

  p_Color.Initialize(0,0,0,255);

  p_ButtonCount = 0;
  p_LabelCount = 0;
  p_TextCount = 0;
  
  p_MouseDown = false;
  p_TitleDown = false;
  p_ResizeDown = false;

  p_FocusedText = 0;

  short x;

  for(x=0;x<49;x++)
  {
    p_Button[x] = 0;
    p_Label[x] = 0;
    p_Text[x] = 0;
  }

}

StandardWindow::~StandardWindow()
{
  Uninitialize();
}

short StandardWindow::Initialize(char *Name, _Rect *WindowSize)
{

  p_Name = Name;

  Resize(WindowSize);

  return 0;
}

void StandardWindow::Uninitialize()
{

  if(p_Window != 0)
  {
    p_Window->Uninitialize();
    delete p_Window;
    p_Window = 0;
  }

  if(p_ClientArea != 0)
  {
    p_ClientArea->Uninitialize();
    delete p_ClientArea;
    p_ClientArea = 0;
  }

  if(p_Border != 0)
  {
    p_Border->Uninitialize();
    delete p_Border;
    p_Border = 0;
  }

  short x;

  for(x=0;x<p_ButtonCount;x++)
  {
    if(p_Button[x] != 0)
    {
      p_Button[x]->Uninitialize();
      delete p_Button[x];
      p_Button[x] = 0;
    }
  }
  for(x=0;x<p_LabelCount;x++)
  {
    if(p_Label[x] != 0)
    {
      p_Label[x]->Uninitialize();
      delete p_Label[x];
      p_Label[x] = 0;
    }
  }
  for(x=0;x<p_TextCount;x++)
  {
    if(p_Text[x] != 0)
    {
      p_Text[x]->Uninitialize();
      delete p_Text[x];
      p_Text[x] = 0;
    }
  }

  p_FocusedText = 0;


  return;
}

unsigned long StandardWindow::Draw()
{
  unsigned long TPF = 0;

  TPF += p_Window->Draw();
  TPF += p_ClientArea->Draw();
  TPF += p_Border->Draw();

  short x;

  for(x=0;x<p_ButtonCount;x++)
  {
    TPF += p_Button[x]->Draw();
  }
  for(x=0;x<p_LabelCount;x++)
  {
    TPF += p_Label[x]->Draw();
  }
  for(x=0;x<p_TextCount;x++)
  {
    TPF += p_Text[x]->Draw();
  }

  p_3D->DrawTextF(p_Name, p_TitleRect, 1, 1, &p_Color);

  return TPF;
}

StandardButton* StandardWindow::CreateButton(unsigned char UniqueID, char *Name,  _Rect *Rect)
{

  p_Button[p_ButtonCount] = new StandardButton(p_3D);

  _Rect NonScaled = *Rect;

  Rect->x1 = (p_ClientAreaRect.x2 - p_ClientAreaRect.x1) * (Rect->x1) + p_ClientAreaRect.x1;
  Rect->x2 = (p_ClientAreaRect.x2 - p_ClientAreaRect.x1) * (Rect->x2) + p_ClientAreaRect.x1;
  Rect->y1 = (p_ClientAreaRect.y2 - p_ClientAreaRect.y1) * (Rect->y1) + p_ClientAreaRect.y1;
  Rect->y2 = (p_ClientAreaRect.y2 - p_ClientAreaRect.y1) * (Rect->y2) + p_ClientAreaRect.y1;

  p_Button[p_ButtonCount]->Initialize(UniqueID, Name, Rect, &NonScaled);

  p_ButtonCount++;

  return p_Button[p_ButtonCount-1];
}

StandardLabel* StandardWindow::CreateLabel(char *Name,  _Rect *Rect)
{

  p_Label[p_LabelCount] = new StandardLabel(p_3D);

  _Rect NonScaled = *Rect;

  Rect->x1 = (p_ClientAreaRect.x2 - p_ClientAreaRect.x1) * (Rect->x1) + p_ClientAreaRect.x1;
  Rect->x2 = (p_ClientAreaRect.x2 - p_ClientAreaRect.x1) * (Rect->x2) + p_ClientAreaRect.x1;
  Rect->y1 = (p_ClientAreaRect.y2 - p_ClientAreaRect.y1) * (Rect->y1) + p_ClientAreaRect.y1;
  Rect->y2 = (p_ClientAreaRect.y2 - p_ClientAreaRect.y1) * (Rect->y2) + p_ClientAreaRect.y1;

  p_Label[p_LabelCount]->Initialize(Name, Rect, &NonScaled);

  p_LabelCount++;

  return p_Label[p_LabelCount-1];
}

StandardLabel* StandardWindow::CreateText(char *Name,  _Rect *Rect)
{

  p_Text[p_LabelCount] = new StandardEdit(p_3D);

  _Rect NonScaled = *Rect;

  Rect->x1 = (p_ClientAreaRect.x2 - p_ClientAreaRect.x1) * (Rect->x1) + p_ClientAreaRect.x1;
  Rect->x2 = (p_ClientAreaRect.x2 - p_ClientAreaRect.x1) * (Rect->x2) + p_ClientAreaRect.x1;
  Rect->y1 = (p_ClientAreaRect.y2 - p_ClientAreaRect.y1) * (Rect->y1) + p_ClientAreaRect.y1;
  Rect->y2 = (p_ClientAreaRect.y2 - p_ClientAreaRect.y1) * (Rect->y2) + p_ClientAreaRect.y1;

  p_Text[p_TextCount]->Initialize(Name, Rect, &NonScaled);

  p_TextCount++;

  return p_Label[p_LabelCount-1];
}

_Rect* StandardWindow::GetCoords()
{
  return &p_Rect;
}

void StandardWindow::Resize(_Rect *WindowSize)
{

  if(p_Window != 0)
  {
    p_Window->Uninitialize();
    delete p_Window;
    p_Window = 0;
  }

  if(p_ClientArea != 0)
  {
    p_ClientArea->Uninitialize();
    delete p_ClientArea;
    p_ClientArea = 0;
  }

  if(p_Border != 0)
  {
    p_Border->Uninitialize();
    delete p_Border;
    p_Border = 0;
  }

  p_Rect = *WindowSize;

  p_Window = p_3D->CreatePolygonFlat();
  p_Border = p_3D->CreatePolygonFlat();
  p_ClientArea = p_3D->CreatePolygonFlat();

  StandardColor color;

  color.Initialize(200,225,255,192);

  p_Window->Initialize(WindowSize->x1,WindowSize->y1,WindowSize->x2,WindowSize->y2, false, &color);

  color.Initialize(0,0,0,20);

  p_ClientAreaRect.x1 = (WindowSize->x1 * p_Init->Width + 4) / p_Init->Width;
  p_ClientAreaRect.x2 = (WindowSize->x2 * p_Init->Width - 4) / p_Init->Width;
  p_ClientAreaRect.y1 = (WindowSize->y1 * p_Init->Height + 22) / p_Init->Height;
  p_ClientAreaRect.y2 = (WindowSize->y2 * p_Init->Height - 22) / p_Init->Height;

  p_ClientArea->Initialize(p_ClientAreaRect.x1 * p_Init->Width, p_ClientAreaRect.y1 * p_Init->Height,
                           p_ClientAreaRect.x2 * p_Init->Width, p_ClientAreaRect.y2 * p_Init->Height,
                           true,
                           &color);

  StandardTLVertex  Vertexes[38];

  Vertexes[0].Initialize( WindowSize->x1 * p_Init->Width , WindowSize->y1 * p_Init->Height,  0,  0,  0,255);
  Vertexes[1].Initialize( WindowSize->x1 * p_Init->Width , WindowSize->y1 * p_Init->Height,   0,  0,  0,255);

  Vertexes[2].Initialize( WindowSize->x2 * p_Init->Width,  WindowSize->y1 * p_Init->Height,    0,  0,  0,255);
  Vertexes[3].Initialize( WindowSize->x2 * p_Init->Width,  WindowSize->y1 * p_Init->Height,    0,  0,  0,255);

  Vertexes[4].Initialize( WindowSize->x1 * p_Init->Width,  WindowSize->y2 * p_Init->Height,    0,  0,  0,255);
  Vertexes[5].Initialize( WindowSize->x1 * p_Init->Width,  WindowSize->y2 * p_Init->Height,    0,  0,  0,255);

  Vertexes[6].Initialize( WindowSize->x2 * p_Init->Width,  WindowSize->y2 * p_Init->Height,    0,  0,  0,255);
  Vertexes[7].Initialize( WindowSize->x2 * p_Init->Width,  WindowSize->y2 * p_Init->Height,    0,  0,  0,255);

  // ---
  Vertexes[8].Initialize( WindowSize->x1 * p_Init->Width + 1,  WindowSize->y1 * p_Init->Height + 1,     255,  255,  255, 255);
  Vertexes[9].Initialize( WindowSize->x1 * p_Init->Width + 1,  WindowSize->y1 * p_Init->Height + 1,     255,  255,  255, 255);

  Vertexes[10].Initialize( WindowSize->x2 * p_Init->Width - 1,  WindowSize->y1 * p_Init->Height + 1,    255,  255,  255, 255);
  Vertexes[11].Initialize( WindowSize->x2 * p_Init->Width - 1,  WindowSize->y1 * p_Init->Height + 1,    0,  0,  0, 50);

  Vertexes[12].Initialize( WindowSize->x1 * p_Init->Width + 1,  WindowSize->y2 * p_Init->Height - 1,    255,  255,  255, 255);
  Vertexes[13].Initialize( WindowSize->x1 * p_Init->Width + 1,  WindowSize->y2 * p_Init->Height - 1,    0,  0,  0, 50);

  Vertexes[14].Initialize( WindowSize->x2 * p_Init->Width - 1,  WindowSize->y2 * p_Init->Height - 1,    0,  0,  0, 50);
  Vertexes[15].Initialize( WindowSize->x2 * p_Init->Width - 1,  WindowSize->y2 * p_Init->Height - 1,    0,  0,  0, 50);

  // ---
  Vertexes[16].Initialize( WindowSize->x1 * p_Init->Width + 3,  WindowSize->y1 * p_Init->Height + 20,   0,  0,  0, 100);
  Vertexes[17].Initialize( WindowSize->x2 * p_Init->Width - 4,  WindowSize->y1 * p_Init->Height + 20,   0,  0,  0, 100);

  Vertexes[18].Initialize( WindowSize->x1 * p_Init->Width + 3,  WindowSize->y1 * p_Init->Height + 21,   255,255,255,100);
  Vertexes[19].Initialize( WindowSize->x2 * p_Init->Width - 4,  WindowSize->y1 * p_Init->Height + 21,   255,255,255,100);


  Vertexes[20].Initialize( WindowSize->x1 * p_Init->Width + 3,  WindowSize->y2 * p_Init->Height - 21,   0,  0,  0, 100);
  Vertexes[21].Initialize( WindowSize->x2 * p_Init->Width - 4,  WindowSize->y2 * p_Init->Height - 21,   0,  0,  0, 100);

  Vertexes[22].Initialize( WindowSize->x1 * p_Init->Width + 3,  WindowSize->y2 * p_Init->Height - 20,   255,255,255,100);
  Vertexes[23].Initialize( WindowSize->x2 * p_Init->Width - 4,  WindowSize->y2 * p_Init->Height - 20,   255,255,255,100);


  Vertexes[24].Initialize( WindowSize->x1 * p_Init->Width + 3,  WindowSize->y1 * p_Init->Height + 20,   0,0,0,100);
  Vertexes[25].Initialize( WindowSize->x1 * p_Init->Width + 3,  WindowSize->y2 * p_Init->Height - 21,   0,0,0,100);

  Vertexes[26].Initialize( WindowSize->x1 * p_Init->Width + 4,  WindowSize->y1 * p_Init->Height + 20,   255,255,255,100);
  Vertexes[27].Initialize( WindowSize->x1 * p_Init->Width + 4,  WindowSize->y2 * p_Init->Height - 21,   255,255,255,100);

  Vertexes[28].Initialize( WindowSize->x2 * p_Init->Width - 4,  WindowSize->y1 * p_Init->Height + 20,   0,0,0,100);
  Vertexes[29].Initialize( WindowSize->x2 * p_Init->Width - 4,  WindowSize->y2 * p_Init->Height - 21,   0,0,0,100);

  Vertexes[30].Initialize( WindowSize->x2 * p_Init->Width - 3,  WindowSize->y1 * p_Init->Height + 20,   255,255,255,100);
  Vertexes[31].Initialize( WindowSize->x2 * p_Init->Width - 3,  WindowSize->y2 * p_Init->Height - 21,   255,255,255,100);

  // --- resize box
  Vertexes[32].Initialize( WindowSize->x2 * p_Init->Width - 18,  WindowSize->y2 * p_Init->Height - 4,     0,0,0,100);
  Vertexes[33].Initialize( WindowSize->x2 * p_Init->Width - 4,   WindowSize->y2 * p_Init->Height - 18,    0,0,0,100);

  Vertexes[34].Initialize( WindowSize->x2 * p_Init->Width - 4,   WindowSize->y2 * p_Init->Height - 4,    255,255,255,100);
  Vertexes[35].Initialize( WindowSize->x2 * p_Init->Width - 4,   WindowSize->y2 * p_Init->Height - 18,   255,255,255,100);

  Vertexes[36].Initialize( WindowSize->x2 * p_Init->Width - 4,   WindowSize->y2 * p_Init->Height -  4,   255,255,255,100);
  Vertexes[37].Initialize( WindowSize->x2 * p_Init->Width - 18,  WindowSize->y2 * p_Init->Height -  4,   255,255,255,100);

  p_TitleRect.x1 = WindowSize->x1;
  p_TitleRect.y1 = WindowSize->y1;
  p_TitleRect.x2 = WindowSize->x2;
  p_TitleRect.y2 = WindowSize->y1 + ((float)20 / (float)p_Init->Height);

  p_ResizeRect.x1 = WindowSize->x2 - ((float)18 / (float)p_Init->Height);
  p_ResizeRect.y1 = WindowSize->y2 - ((float)18 / (float)p_Init->Height);
  p_ResizeRect.x2 = WindowSize->x2 - ((float)4 / (float)p_Init->Height);
  p_ResizeRect.y2 = WindowSize->y2 - ((float)4 / (float)p_Init->Height);

  p_Border->Initialize(FW_LINES, true, 38, Vertexes);

  short Indx[38];

  Indx[0] = 0;
  Indx[1] = 2;

  Indx[2] = 3;
  Indx[3] = 6;

  Indx[4] = 5;
  Indx[5] = 7;

  Indx[6] = 1;
  Indx[7] = 4;


  Indx[8] = 8;
  Indx[9] = 10;

  Indx[10] = 11;
  Indx[11] = 14;

  Indx[12] = 13;
  Indx[13] = 15;

  Indx[14] = 9;
  Indx[15] = 12;

  // ---

  Indx[16] = 16;
  Indx[17] = 17;

  Indx[18] = 18;
  Indx[19] = 19;

  Indx[20] = 20;
  Indx[21] = 21;

  Indx[22] = 22;
  Indx[23] = 23;

  Indx[24] = 24;
  Indx[25] = 25;

  Indx[26] = 26;
  Indx[27] = 27;

  Indx[28] = 28;
  Indx[29] = 29;

  Indx[30] = 30;
  Indx[31] = 31;

  // resize

  Indx[32] = 32;
  Indx[33] = 33;

  Indx[34] = 34;
  Indx[35] = 35;

  Indx[36] = 36;
  Indx[37] = 37;

  p_Border->SetIndexBuffer(Indx, 38);

  // ----

  unsigned char x;

  // hit window
  for(x=0;x<p_ButtonCount;x++)
  {
    if(p_Button[x] != 0)
    {

      _Rect Rect;

      Rect = *p_Button[x]->GetNonScaledCoords();

      Rect.x1 = ((p_ClientAreaRect.x2 - p_ClientAreaRect.x1) * (Rect.x1)) + p_ClientAreaRect.x1;
      Rect.x2 = ((p_ClientAreaRect.x2 - p_ClientAreaRect.x1) * (Rect.x2)) + p_ClientAreaRect.x1;
      Rect.y1 = ((p_ClientAreaRect.y2 - p_ClientAreaRect.y1) * (Rect.y1)) + p_ClientAreaRect.y1;
      Rect.y2 = ((p_ClientAreaRect.y2 - p_ClientAreaRect.y1) * (Rect.y2)) + p_ClientAreaRect.y1;

      p_Button[x]->Resize(&Rect);

    }
  }
  for(x=0;x<p_LabelCount;x++)
  {
    if(p_Label[x] != 0)
    {

      _Rect Rect;

      Rect = *p_Label[x]->GetNonScaledCoords();

      Rect.x1 = ((p_ClientAreaRect.x2 - p_ClientAreaRect.x1) * (Rect.x1)) + p_ClientAreaRect.x1;
      Rect.x2 = ((p_ClientAreaRect.x2 - p_ClientAreaRect.x1) * (Rect.x2)) + p_ClientAreaRect.x1;
      Rect.y1 = ((p_ClientAreaRect.y2 - p_ClientAreaRect.y1) * (Rect.y1)) + p_ClientAreaRect.y1;
      Rect.y2 = ((p_ClientAreaRect.y2 - p_ClientAreaRect.y1) * (Rect.y2)) + p_ClientAreaRect.y1;

      p_Label[x]->Resize(&Rect);

    }
  }

  for(x=0;x<p_TextCount;x++)
  {
    if(p_Text[x] != 0)
    {

      _Rect Rect;

      Rect = *p_Text[x]->GetNonScaledCoords();

      Rect.x1 = ((p_ClientAreaRect.x2 - p_ClientAreaRect.x1) * (Rect.x1)) + p_ClientAreaRect.x1;
      Rect.x2 = ((p_ClientAreaRect.x2 - p_ClientAreaRect.x1) * (Rect.x2)) + p_ClientAreaRect.x1;
      Rect.y1 = ((p_ClientAreaRect.y2 - p_ClientAreaRect.y1) * (Rect.y1)) + p_ClientAreaRect.y1;
      Rect.y2 = ((p_ClientAreaRect.y2 - p_ClientAreaRect.y1) * (Rect.y2)) + p_ClientAreaRect.y1;

      p_Text[x]->Resize(&Rect);

    }
  }

  return;
}

unsigned char StandardWindow::Process(_GUI_KEY *key, _GUI_MOUSE *mouse)
{
  
  unsigned char x;

  if(key!=0)
  {
    if(p_FocusedText != 0)
    {
      
      char tempLetter;

      switch(key->key)
      {
        case 2:
          if(key->shift == true)
          {
            tempLetter = '!';
          }
          else
          {
            tempLetter = '1';
          }
          break;
        case 3:
          if(key->shift == true)
          {
            tempLetter = '@';
          }
          else
          {
            tempLetter = '2';
          }
          break;
        case 4:
          if(key->shift == true)
          {
            tempLetter = '#';
          }
          else
          {
            tempLetter = '3';
          }
          break;
        case 5:
          if(key->shift == true)
          {
            tempLetter = '$';
          }
          else
          {
            tempLetter = '4';
          }
          break;
        case 6:
          if(key->shift == true)
          {
            tempLetter = '%';
          }
          else
          {
            tempLetter = '5';
          }
          break;
        case 7:
          if(key->shift == true)
          {
            tempLetter = '^';
          }
          else
          {
            tempLetter = '6';
          }
          break;
        case 8:
          if(key->shift == true)
          {
            tempLetter = '&';
          }
          else
          {
            tempLetter = '7';
          }
          break;
        case 9:
          if(key->shift == true)
          {
            tempLetter = '*';
          }
          else
          {
            tempLetter = '8';
          }
          break;
        case 10:
          if(key->shift == true)
          {
            tempLetter = '(';
          }
          else
          {
            tempLetter = '9';
          }
          break;
        case 11:
          if(key->shift == true)
          {
            tempLetter = ')';
          }
          else
          {
            tempLetter = '0';
          }
          break;
        case 12:
          if(key->shift == true)
          {
            tempLetter = '_';
          }
          else
          {
            tempLetter = '-';
          }
          break;
        case 13:
          if(key->shift == true)
          {
            tempLetter = '+';
          }
          else
          {
            tempLetter = '=';
          }
          break;
        case 16:
          if(key->shift == true)
          {
            tempLetter = 'Q';
          }
          else
          {
            tempLetter = 'q';
          }
          break;
        case 17:
          if(key->shift == true)
          {
            tempLetter = 'W';
          }
          else
          {
            tempLetter = 'w';
          }
          break;
        case 18:
          if(key->shift == true)
          {
            tempLetter = 'E';
          }
          else
          {
            tempLetter = 'e';
          }
          break;
        case 19:
          if(key->shift == true)
          {
            tempLetter = 'R';
          }
          else
          {
            tempLetter = 'r';
          }
          break;
        case 20:
          if(key->shift == true)
          {
            tempLetter = 'T';
          }
          else
          {
            tempLetter = 't';
          }
          break;
        case 21:
          if(key->shift == true)
          {
            tempLetter = 'Y';
          }
          else
          {
            tempLetter = 'y';
          }
          break;
        case 22:
          if(key->shift == true)
          {
            tempLetter = 'U';
          }
          else
          {
            tempLetter = 'u';
          }
          break;
        case 23:
          if(key->shift == true)
          {
            tempLetter = 'I';
          }
          else
          {
            tempLetter = 'i';
          }
          break;
        case 24:
          if(key->shift == true)
          {
            tempLetter = 'O';
          }
          else
          {
            tempLetter = 'o';
          }
          break;
        case 25:
          if(key->shift == true)
          {
            tempLetter = 'P';
          }
          else
          {
            tempLetter = 'p';
          }
          break;
        case 26:
          if(key->shift == true)
          {
            tempLetter = '{';
          }
          else
          {
            tempLetter = '[';
          }
          break;
        case 27:
          if(key->shift == true)
          {
            tempLetter = '}';
          }
          else
          {
            tempLetter = ']';
          }
          break;
        case 30:
          if(key->shift == true)
          {
            tempLetter = 'A';
          }
          else
          {
            tempLetter = 'a';
          }
          break;
        case 31:
          if(key->shift == true)
          {
            tempLetter = 'S';
          }
          else
          {
            tempLetter = 's';
          }
          break;
        case 32:
          if(key->shift == true)
          {
            tempLetter = 'D';
          }
          else
          {
            tempLetter = 'd';
          }
          break;
        case 33:
          if(key->shift == true)
          {
            tempLetter = 'F';
          }
          else
          {
            tempLetter = 'f';
          }
          break;
        case 34:
          if(key->shift == true)
          {
            tempLetter = 'G';
          }
          else
          {
            tempLetter = 'g';
          }
          break;
        case 35:
          if(key->shift == true)
          {
            tempLetter = 'H';
          }
          else
          {
            tempLetter = 'h';
          }
          break;
        case 36:
          if(key->shift == true)
          {
            tempLetter = 'J';
          }
          else
          {
            tempLetter = 'j';
          }
          break;
        case 37:
          if(key->shift == true)
          {
            tempLetter = 'K';
          }
          else
          {
            tempLetter = 'k';
          }
          break;
        case 38:
          if(key->shift == true)
          {
            tempLetter = 'L';
          }
          else
          {
            tempLetter = 'l';
          }
          break;
        case 39:
          if(key->shift == true)
          {
            tempLetter = ':';
          }
          else
          {
            tempLetter = ';';
          }
          break;
        case 40:
          if(key->shift == true)
          {
            tempLetter = '"';
          }
          else
          {
            tempLetter = '\'';
          }
          break;
        case 41:
          if(key->shift == true)
          {
            tempLetter = '~';
          }
          else
          {
            tempLetter = '`';
          }
          break;
        case 43:
          if(key->shift == true)
          {
            tempLetter = '|';
          }
          else
          {
            tempLetter = '\\';
          }
          break;
        case 44:
          if(key->shift == true)
          {
            tempLetter = 'Z';
          }
          else
          {
            tempLetter = 'z';
          }
          break;
        case 45:
          if(key->shift == true)
          {
            tempLetter = 'X';
          }
          else
          {
            tempLetter = 'x';
          }
          break;
        case 46:
          if(key->shift == true)
          {
            tempLetter = 'C';
          }
          else
          {
            tempLetter = 'c';
          }
          break;
        case 47:
          if(key->shift == true)
          {
            tempLetter = 'V';
          }
          else
          {
            tempLetter = 'v';
          }
          break;
        case 48:
          if(key->shift == true)
          {
            tempLetter = 'B';
          }
          else
          {
            tempLetter = 'b';
          }
          break;
        case 49:
          if(key->shift == true)
          {
            tempLetter = 'N';
          }
          else
          {
            tempLetter = 'n';
          }
          break;
        case 50:
          if(key->shift == true)
          {
            tempLetter = 'M';
          }
          else
          {
            tempLetter = 'm';
          }
          break;
        case 51:
          if(key->shift == true)
          {
            tempLetter = '<';
          }
          else
          {
            tempLetter = ',';
          }
          break;
        case 52:
          if(key->shift == true)
          {
            tempLetter = '>';
          }
          else
          {
            tempLetter = '.';
          }
          break;
        case 53:
          if(key->shift == true)
          {
            tempLetter = '?';
          }
          else
          {
            tempLetter = '/';
          }
          break;
        case 55:
          tempLetter = '*';
          break;
        case 57:
          tempLetter = ' ';
          break;
        case 71:
          tempLetter = '7';
          break;
        case 72:
          tempLetter = '8';
          break;
        case 73:
          tempLetter = '9';
          break;
        case 74:
          tempLetter = '-';
          break;
        case 75:
          tempLetter = '4';
          break;
        case 76:
          tempLetter = '5';
          break;
        case 77:
          tempLetter = '6';
          break;
        case 78:
          tempLetter = '+';
          break;
        case 79:
          tempLetter = '1';
          break;
        case 80:
          tempLetter = '2';
          break;
        case 81:
          tempLetter = '3';
          break;
        case 82:
          tempLetter = '0';
          break;
        case 83:
          tempLetter = '.';
          break;
        case 181:
          tempLetter = '/';
          break;
        default:
          tempLetter = 0;
      }

      char *tempString;

      switch(key->key)
      {
        case 14: // backspace

          unsigned long tempLen;

          tempLen = strlen(p_FocusedText->GetCaption());

          tempString = new char[tempLen + 1];

          strcpy(tempString, p_FocusedText->GetCaption());

          if(tempLen > 0)
          {
            tempString[tempLen - 1] = 0;
          }

          p_FocusedText->SetCaption(tempString);

          delete[]tempString;

          break;
        case 28: // enter
        case 156: // enter on key
          // set unfocused
          p_FocusedText->SetDown(false); // set focused.
          p_FocusedText = 0;

          break; 
        default:
          if(tempLetter != 0)
          {
            tempString = new char[strlen(p_FocusedText->GetCaption()) + 2];

            strcpy(tempString, p_FocusedText->GetCaption());

            tempString[strlen(p_FocusedText->GetCaption())] = tempLetter;

            tempString[strlen(p_FocusedText->GetCaption()) + 1] = 0;

            p_FocusedText->SetCaption(tempString);

            delete[]tempString;
          }
          break;
      }

    }
  }

  if(mouse == 0)
  {
    return -1;
  }

  if(p_MouseDown == true && mouse->down == false)
  {
    for(x=0;x<p_ButtonCount;x++)
    {
      p_Button[x]->SetDown(false);
    }
    p_TitleDown = false;
    p_ResizeDown = false;
  }

  if(p_MouseDown == false &&  mouse->down == true)
  {
    // check hit title
    if(mouse->pt.x > p_TitleRect.x1 && mouse->pt.x < p_TitleRect.x2 &&
       mouse->pt.y > p_TitleRect.y1 && mouse->pt.y < p_TitleRect.y2)
    {
      p_TitleDown = true;

      p_OffSet.x = mouse->pt.x - p_Rect.x1;
      p_OffSet.y = mouse->pt.y - p_Rect.y1;
    }

    // check to see if they are resizing
    if(mouse->pt.x > p_ResizeRect.x1 && mouse->pt.x < p_ResizeRect.x2 &&
       mouse->pt.y > p_ResizeRect.y1 && mouse->pt.y < p_ResizeRect.y2)
    {
      p_ResizeDown = true;

      p_OffSet.x = mouse->pt.x - p_Rect.x2;
      p_OffSet.y = mouse->pt.y - p_Rect.y2;
    }

    // check to see if they focused an edit box.
    p_FocusedText = 0;

    for(x=0;x<p_TextCount;x++)
    {
      _Rect *Rect;
      
      Rect = p_Text[x]->GetCoords();

      if(mouse->pt.x > Rect->x1 && mouse->pt.x < Rect->x2 &&
         mouse->pt.y > Rect->y1 && mouse->pt.y < Rect->y2)
      {
        p_FocusedText = p_Text[x];
        p_FocusedText->SetDown(true); // set focused.
      }
    }

  }

  p_MouseDown =  mouse->down; 

  if(p_TitleDown == true)
  {

    _Rect tempRect;

    tempRect = p_Rect;

    tempRect.x1 += (mouse->pt.x - (p_OffSet.x + p_Rect.x1));
    tempRect.x2 += (mouse->pt.x - (p_OffSet.x + p_Rect.x1));
    tempRect.y1 += (mouse->pt.y - (p_OffSet.y + p_Rect.y1));
    tempRect.y2 += (mouse->pt.y - (p_OffSet.y + p_Rect.y1));


    // check boundaries
    float diff;

    if(tempRect.x1 < 0)
    {
      diff = 0 - tempRect.x1;
      
      tempRect.x1 += diff;
      tempRect.x2 += diff;
    }

    if(tempRect.x2 > 1)
    {
      diff = 1 - tempRect.x2;
      
      tempRect.x1 += diff;
      tempRect.x2 += diff;
    }

    if(tempRect.y1 < 0)
    {
      diff = 0 - tempRect.y1;
      
      tempRect.y1 += diff;
      tempRect.y2 += diff;
    }

    if(tempRect.y2 > 1)
    {
      diff = 1 - tempRect.y2;
      
      tempRect.y1 += diff;
      tempRect.y2 += diff;
    }

    Resize(&tempRect);
  }

  
  if(p_ResizeDown == true)
  {

    _Rect tempRect;

    tempRect = p_Rect;

    tempRect.x2 += (mouse->pt.x - (p_OffSet.x + p_Rect.x2));
    tempRect.y2 += (mouse->pt.y - (p_OffSet.y + p_Rect.y2));


    // check boundaries
    float diff;

    if(tempRect.x2 < (tempRect.x1 + 0.2f))
    {
      diff = (tempRect.x1 + 0.2f) - tempRect.x2;
      
      tempRect.x2 += diff;
    }

    if(tempRect.x2 > 1)
    {
      diff = 1 - tempRect.x2;
      
      tempRect.x2 += diff;
    }

    if(tempRect.y2 < (tempRect.y1 + 0.2f))
    {
      diff = (tempRect.y1 + 0.2f) - tempRect.y2;
      
      tempRect.y2 += diff;
    }

    if(tempRect.y2 > 1)
    {
      diff = 1 - tempRect.y2;
      
      tempRect.y2 += diff;
    }

    Resize(&tempRect);
  }


  if(p_ResizeDown == false && p_TitleDown == false)
  {
    // check hit window
    if(mouse->pt.x > p_Rect.x1 && mouse->pt.x < p_Rect.x2 &&
       mouse->pt.y > p_Rect.y1 && mouse->pt.y < p_Rect.y2)
    {   

      // check hit button
      for(x=0;x<p_ButtonCount;x++)
      {
        if(p_Button[x] != 0)
        {
          _Rect *Rect;

          Rect = p_Button[x]->GetCoords();

          if(mouse->pt.x > Rect->x1 && mouse->pt.x < Rect->x2 &&
             mouse->pt.y > Rect->y1 && mouse->pt.y < Rect->y2)
          {
            // hit it
            if(mouse->down == true)
            {
              p_Button[x]->SetDown(true);
            }
            else
            {
              p_Button[x]->SetDown(false);
            }
            return p_Button[x]->GetUniqueID();
          }
          else
          {
            p_Button[x]->SetDown(false);
          }
        }
      }
    }
  }

  return -1;

}