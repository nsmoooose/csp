// Utilities.h : Header-Datei zu TerrainUtils.cpp
//


#if !defined(tools_h)
#define tools_h

#include "stdafx.h"
#include "globalvars.h"

typedef unsigned char BYTE;
typedef long LONG;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef void* LPVOID;


typedef struct tagMYRGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} MYRGBQUAD;


typedef struct tagMYBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} MYBITMAPINFOHEADER, *LPMYBITMAPINFOHEADER;

/* Bitmap Header Definition */
typedef struct tagMYBITMAP
  {
    LONG        bmType;
    LONG        bmWidth;
    LONG        bmHeight;
    LONG        bmWidthBytes;
    WORD        bmPlanes;
    WORD        bmBitsPixel;
    LPVOID      bmBits;
  } MYBITMAP, *LPMYBITMAP;


typedef struct tagMYBITMAPINFO {
    MYBITMAPINFOHEADER    bmiHeader;
    MYRGBQUAD             bmiColors[1];
} MYBITMAPINFO, *LPMYBITMAPINFO;

typedef struct tagMYBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} MYBITMAPFILEHEADER, *LPMYBITMAPFILEHEADER;



void		wexit(int code);
LPBITMAP	LoadBmpIn8BppBuffer(char* filename);

#endif