// Combat Simulator Project - RAWExtract - RAW Data Extraction Tool
// Copyright (C) 2002 The Combat Simulator Project 
// http://csp.sourceforge.net 
//  
// This program is free software; you can redistribute it and/or 
// modify it under the terms of the GNU General Public License 
// as published by the Free Software Foundation; either version 2 
// of the License, or (at your option) any later version. 
//  
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
// GNU General Public License for more details. 
//  
// You should have received a copy of the GNU General Public License 
// along with this program; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. 

/** 
 * This tool extracts a block of any size as a file of its
 * own from a larger RAW file. The resulting file can either be imported
 * Photoshop or any other image processing program, or converted to text
 * using RAW2txt.
 *
 * @author Martin "Stormbringer" Hoffmann <storm_bringer@gmx.li> 
 */ 

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "io.h"
#include "fcntl.h"
#include "SYS\STAT.H"
#include "math.h"

short *aiHeightmap, *fract_map;
int iWidth, iHeight;
char cSourceFileName[256], cDestFileName[256], cPosX[256], cPosY[256], cStringBuffer[256], cSize[256];
long iSourceFileHandle, iDestFileHandle;

int LoadTerrain(int iFile, int start_x, int start_y, int dest_width)
{
	int success;
	long lsize;
	short *buffer;
	float fwidth;

	int iWidth, iHeight;

	// Load SCN data and save to internal structure

	lsize = _filelength(iFile);
	lsize /= 2;									// SCN uses 16bit height values, so we have to divide by 2
												// to get the real number of height map values
	fwidth = (float)sqrt((double)lsize);		// compute terrain width from size of file
	iWidth = (int)fwidth;						
	iHeight = (int)fwidth;						

	aiHeightmap = new short[lsize];
	buffer = new short[lsize];
	fract_map = new short[dest_width*dest_width];

	success = _read(iFile, buffer, (lsize*sizeof(short)));

	for (int y=start_y; y<start_y+dest_width; y++)					
		for (int x=start_x; x<start_x+dest_width; x++)
		{
			fract_map[(y-start_y)*dest_width+x-start_x] =  (short)buffer[y*iHeight+x];
		} 

	delete []buffer;
	return iWidth;
}

int SaveTerrain(int iDestFileHandle, int size)
{
	int success;

	success = _write(iDestFileHandle, fract_map, size);
	_chmod(cDestFileName, _S_IREAD | _S_IWRITE);
	_close(iDestFileHandle);

	return 1;
}


int main(int argc, char* argv[])
{
	printf("CSP - RAW Data Extractor\n"
		   "Copyright (C) 2002 The Combat Simulator Project\n"
		   "http://csp.sourceforge.net\n\n");

	if(argc < 5)
	{
		printf("ERROR: Not enough parameters!\n\n"
			   "Usage: RAWExtract sourcefile destfile XPos YPos Size\n\n");
		return -1;		
	}

	// Source and Destination file names
	strcpy(cSourceFileName, argv[1]);
	strcpy(cDestFileName, argv[2]);
	strcpy(cPosX, argv[3]);
	strcpy(cPosY, argv[4]);
	strcpy(cSize, argv[5]);

	iSourceFileHandle = _open(cSourceFileName, _O_BINARY);
	if(iSourceFileHandle==-1)
	{	
		printf("unable to open '%s'!\n", cSourceFileName);
		char text[100];
		gets(text);
		return -1;
	}

	iDestFileHandle = _open(cDestFileName, _O_CREAT | _O_RDWR | _O_BINARY, _S_IREAD | _S_IWRITE);
	if(iDestFileHandle==-1)
	{	
		printf("unable to open '%s'!\n", cDestFileName);
		char text[100];
		gets(text);
		_close(iSourceFileHandle);
		return -1;
	}

	LoadTerrain(iSourceFileHandle, atoi(cPosX), atoi(cPosY), atoi(cSize));
	SaveTerrain(iDestFileHandle, sizeof(short)*(atoi(cSize)*atoi(cSize)));

	_close(iSourceFileHandle);
	_close(iDestFileHandle);

	return 0;
}
