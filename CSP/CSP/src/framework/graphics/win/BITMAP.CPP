/*
 *		24BPP Bitmap loader
 *		Tried to be generic so linux people could use
 *		these routines.  I assume we are using 24BPP
 *		but I can easily add support for 8BPP.
 */

#include "bitmap.h"
#include <fstream.h>
#include <string.h>

/*	info must be a non-null pointer
 *	shouldn't return anything.  BMP is stored in
 *	info->rgba.
 */
void LoadDIBitmap( char *filename, DIBINFO *info )
{
	int i, j, imageSize, qIndex, pIndex;
	unsigned char *pixbuf;
	unsigned char *quadbuf;
	unsigned char *buf = new unsigned char[54];
	DIBFILEHEADER	*fheader = new DIBFILEHEADER;
	DIBINFOHEADER	*iheader = new DIBINFOHEADER;
	ifstream inDIB;
	
	inDIB.open( filename, ios::in | ios::binary | ios::nocreate );

	if( !inDIB.is_open() )
	{
		return;
	}

	inDIB.read( buf, 54 );

	fheader = (DIBFILEHEADER *)memcpy( fheader, buf, 14 );
	iheader = (DIBINFOHEADER *)memcpy( iheader, (buf + 14), 40 );

	imageSize = iheader->biHeight * iheader->biWidth * 3;

	pixbuf = new unsigned char[ imageSize ];

	quadbuf = new unsigned char[ iheader->biHeight * iheader->biWidth * 4 ];

	inDIB.read( pixbuf, iheader->biWidth * iheader->biHeight * 3 );

	/*	DIB's are stored left to right, bottom to top.
	 *	quadbuf is stored left to right, top to bottom,
	 *	rgba.
	 */
	qIndex = 0;
	for( i = 0; i < iheader->biWidth; i++ )
	{
		for( j = iheader->biHeight - 1; j >= 0; j--, qIndex += 4 )
		{
			pIndex = (j*iheader->biWidth*3)+(i*3);
			quadbuf[qIndex+3] = 255;
			quadbuf[qIndex+2] = pixbuf[pIndex];
			quadbuf[qIndex+1] = pixbuf[pIndex+1];
			quadbuf[qIndex  ] = pixbuf[pIndex+2];
		}
	}

	info->biHeader = iheader;
	info->rgba = quadbuf;

	inDIB.close();
	delete( buf );
	delete( pixbuf );
	delete( fheader );
}
