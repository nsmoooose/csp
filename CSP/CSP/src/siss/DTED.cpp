#include "dted.h"

DTED::DTED()
{
  memset(&data, 0, sizeof(data));
}

DTED::~DTED()
{

  short y;

  for(y=0;y<width;y++)
  {
   if (data.drf[y].value != 0) delete[]data.drf[y].value; data.drf[y].value = 0;
  }

  if (data.drf != 0) delete[]data.drf; data.drf = 0;
}


short DTED::ReadFile(char *filename)
{

  FILE *file;
  
  file = fopen(filename, "rb");

  fread(&data.uhr, UHRSize, 1, file);
  fread(&data.dsi, DSISize, 1, file);
  fread(&data.acc, ACCSize, 1, file);

  char tempw[5];
  char temph[5];

  memset(tempw, 0, sizeof(tempw));
  memset(temph, 0, sizeof(temph));

  memcpy(tempw, data.dsi.latilines, sizeof(data.dsi.latilines));
  memcpy(temph, data.dsi.longlines, sizeof(data.dsi.longlines));

  width = atol(tempw);
  height = atol(temph);

  data.drf = new DRF[width];

  short y;
  
  for(y=0;y<width;y++)
  {

    fread(&data.drf[y].sentinel, 8, 1, file);

    data.drf[y].value = new short[height];

    fread(data.drf[y].value, sizeof(short), height, file);

    // swap short file bytes for windows
    _swab((char *)data.drf[y].value, (char *)data.drf[y].value, (height * sizeof(short)));

    fread(data.drf[y].checksum, 4, 1, file);

  }


  fclose(file);

  return 0;

}

short DTED::GetValue(short x, short z)
{
  return data.drf[x].value[z];
}

short DTED::GetWidth()
{
  return (short)width;
}

short DTED::GetHeight()
{
  return (short)height;
}
