
#ifndef	__TGAINFO_H__
#define __TGAINFO_H__

#pragma pack(1)

struct TGAHEADER
{
	unsigned char		IDLength;
	unsigned char		CMapType;
	unsigned char		ImageType;
	unsigned short		EntryIndex;
	unsigned short		CMapLength;
	unsigned char		CMapEntrySize;
	unsigned short		XOrigin;
	unsigned short		YOrigin;
	unsigned short		Width;
	unsigned short		Height;
	unsigned char		PixelDepth;
	unsigned char		ImgDesc;
};

#pragma pack()

struct TGAINFO
{
	unsigned short	usWidth;
	unsigned short	usHeight;
	unsigned int	uiextFormat;
	unsigned char	*pixels;
};

#endif
