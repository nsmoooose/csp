// Utilities.cpp : general functions, commonly used
//


#include "stdafx.h"
#include "Utilities.h"



/*
 * wexit :  Exits Programm, but waits until ENTER is pressed
 */
void wexit(int code)
{
	char text[100];
	gets(text);
	exit(code);
}


/*
 * LoadBmpIn8BppBuffer :  Loads a BMP-file in an array with 8 bpp
 */
LPBITMAP LoadBmpIn8BppBuffer(char* filename)
{
	LPBITMAP			lpbitmap;
	HANDLE				hFile;
	BITMAPFILEHEADER	*pbmfHeader;
	LPBITMAPINFO		bmi;
	DWORD				gelesen;
	LPBYTE				row;
	DWORD				bufcount;
	DWORD				byte_width;

	if (DEBUG_MODE)
	{
		printf("LoadBmpIn8BppBuffer(%s)\n", filename);
	}

	hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE) 
	{
		printf("LoadBmpIn8BppBuffer :  file not found");
		wexit(1);
	}

	pbmfHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
	ReadFile(hFile, pbmfHeader, sizeof(BITMAPFILEHEADER), &gelesen, NULL);
	if (gelesen != sizeof(BITMAPFILEHEADER))
	{
		printf("LoadBmpIn8BppBuffer :  invalid file");
		wexit(1);
	}

	if (pbmfHeader->bfType != ((WORD) ('M' << 8) | 'B'))
	{
		printf("LoadBmpIn8BppBuffer :  invalid file");
		wexit(1);
	}
	bmi = (LPBITMAPINFO)new BYTE[pbmfHeader->bfOffBits];

	ReadFile(hFile, bmi, pbmfHeader->bfOffBits-sizeof(BITMAPFILEHEADER), &gelesen, NULL);
	if (gelesen != pbmfHeader->bfOffBits-sizeof(BITMAPFILEHEADER)) 
	{
		CloseHandle(hFile);
		printf("LoadBmpIn8BppBuffer :  invalid file");
		wexit(1);
	}
	if (bmi->bmiHeader.biPlanes != 1 || bmi->bmiHeader.biBitCount != 8)
	{
		CloseHandle(hFile);
		printf("LoadBmpIn8BppBuffer :  invalid file");
		wexit(1);
	}
	lpbitmap = new BITMAP;
	memset(lpbitmap, 0, sizeof(BITMAP));
	lpbitmap->bmBits = new BYTE[bmi->bmiHeader.biWidth*bmi->bmiHeader.biHeight];

	byte_width = bmi->bmiHeader.biSizeImage/bmi->bmiHeader.biHeight;
	row = new BYTE[byte_width];
	if (DEBUG_MODE)
	{
		printf("bwidth: %d, bh: %d\n", bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight);
	}
	bufcount = 0;
	if (bmi->bmiHeader.biHeight<0)
	{

		for (int i=0; i<-bmi->bmiHeader.biHeight; i++) 
		{
			ReadFile(hFile, row, byte_width, &gelesen, NULL);
			if (gelesen==byte_width)
			{
				for (int x=0; x<bmi->bmiHeader.biWidth; x++)
				{
					((LPBYTE)lpbitmap->bmBits)[bmi->bmiHeader.biWidth*i+x] = row[x*3];
				}
			} else
			{
				printf("LoadBmpIn8BppBuffer :  invalid file");
				wexit(1);
			}
		}
	} else 
	{
		for (int i=bmi->bmiHeader.biHeight-1; i>=0; i--)
		{
			ReadFile(hFile, row, byte_width, &gelesen, NULL);
			if (gelesen==byte_width)
			{
				for (int x=0; x<bmi->bmiHeader.biWidth; x++)
				{
					((LPBYTE)lpbitmap->bmBits)[bmi->bmiHeader.biWidth*i+x] = row[x];
				}
			} else
			{
				printf("LoadBmpIn8BppBuffer :  invalid file");
				wexit(1);
			}
		}
	}
	lpbitmap->bmWidth = bmi->bmiHeader.biWidth;
	lpbitmap->bmHeight = bmi->bmiHeader.biHeight;
	lpbitmap->bmWidthBytes = bmi->bmiHeader.biWidth;
	lpbitmap->bmPlanes = 1;
	lpbitmap->bmBitsPixel = 8;
	CloseHandle(hFile);
	delete bmi;
	delete pbmfHeader;
	delete []row;
	return lpbitmap;
}
