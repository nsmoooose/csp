/*
 *
 *		Bitmap file loader
 *		Redefined from windows.h for the Linux port
 *
 */

#ifndef __BITMAP_H_
#define __BITMAP_H_

#include "Framework.h"

#pragma pack(1)

struct DIBFILEHEADER
{
	unsigned short		bfType;
	unsigned long		bfSize;
	unsigned short		res1;
	unsigned short		res2;
	unsigned long		dataOffset;
};

struct DIBINFOHEADER
{
	unsigned long		biSize;
	long				biWidth;
	long				biHeight;
	unsigned short		biPlanes;
	unsigned short		biBitCount;
	unsigned long		biCompression;
	unsigned long		biImageSize;
	long				biXPPM;
	long				biYPPM;
	unsigned long		biColorsUsed;
	unsigned long		biColorsImportant;
};

#pragma pack()

struct DIBINFO
{
	unsigned int		*texnum;
	DIBINFOHEADER		*biHeader;
	unsigned char		*rgba;

	DIBINFO()
	{
		texnum = new unsigned int(0);
		biHeader = NULL;
		rgba = NULL;
	}

	virtual ~DIBINFO()
	{
		delete( texnum );
		delete( biHeader );
		delete( rgba );
	}
};

#endif
