// Combat Simulator Project - RAW2txt - writes RAW data to a text file
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
 * This tool can be used to verify the result of the fractal
 * generator. It converts the binary data from a RAW file
 * to text. This text file can either be opened with an editor,
 * or the data can be imported to Excel. This was used during
 * the development of the fractal tool, mostly to track down
 * bugs by analyzing the output of the fractal routines.
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

int LoadRAWTerrain(int iFile)
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
	fract_map = new short[lsize];

	success = _read(iFile, buffer, (lsize*sizeof(short)));

	for (int y=0; y<iWidth; y++)					
		for (int x=0; x<iWidth; x++)
		{
			fract_map[y*iWidth+x] =  (short)buffer[y*iWidth+x];
		} 

	delete []buffer;
	return iWidth;
}

/** 
 * ConvertRAWtoTXT - writes RAW data to a text file
 * 
 * This function can be used to verify the result of the fractal
 * generator. It converts the binary data from a RAW file
 * to text. This text file can either be opened with an editor,
 * or the data can be imported to Excel. This was used during
 * the development of the fractal tool, mostly to track down
 * bugs by analyzing the output of the fractal routines.
 *
 * @author Martin "Stormbringer" Hoffmann <storm_bringer@gmx.li> 
 */ 

int ConvertRAWtoTXT(int iDestFileHandle, int iSourceFileHandle)
{
	char cValue[100], cBuffer[100];
	int countX, countY, intbuffer, Success;
	int uiCurrentQuadWidth = (int)(sqrt((float)_filelength(iSourceFileHandle)/2));

	for(countY=0;countY<uiCurrentQuadWidth;countY++)
	{
		for(countX=0;countX<uiCurrentQuadWidth;countX++)
		{
				intbuffer = (int)fract_map[countY*uiCurrentQuadWidth+countX];
				cBuffer[0] = (char)0;
				itoa(intbuffer, cValue, 10);
				int diff=0;
				diff = 6 - strlen(cValue);
				for(int i=0;i<diff;i++)
				{
					strcat(cBuffer, "0");
				}
				if(intbuffer < 0)
				{
					_strnset(cBuffer, '-', 1);
					_strnset(cValue, '0', 1);
				}
				else
				{
					_strnset(cBuffer, ' ', 1);
				}
				strcat(cBuffer, cValue);
				Success = _write(iDestFileHandle, cBuffer, strlen(cBuffer));
				strcpy(cValue, ", ");
				Success = _write(iDestFileHandle, cValue, strlen(cValue));
			}
			strcpy(cValue, "\n");
			Success = write(iDestFileHandle, cValue, strlen(cValue));
		}

	_chmod(cDestFileName, _S_IREAD | _S_IWRITE);
	_close(iDestFileHandle);

	return 1;
}


int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("Not enough parameters!\n");
		return -1;
	}

	// Source and Destination file names
	strcpy(cSourceFileName, argv[1]);
	strcpy(cDestFileName, argv[2]);

	iSourceFileHandle = _open(cSourceFileName, _O_BINARY);
	if(iSourceFileHandle==-1)
	{	
		printf("unable to open '%s'!\n", cSourceFileName);
		char text[100];
		gets(text);
		return -1;
	}

	iDestFileHandle = _open(cDestFileName, _O_CREAT | _O_RDWR, _S_IREAD | _S_IWRITE);
	if(iDestFileHandle==-1)
	{	
		printf("unable to open '%s'!\n", cDestFileName);
		char text[100];
		gets(text);
		_close(iSourceFileHandle);
		return -1;
	}

	LoadRAWTerrain(iSourceFileHandle);
	ConvertRAWtoTXT(iDestFileHandle, iSourceFileHandle);

	_close(iSourceFileHandle);
	_close(iDestFileHandle);

	return 0;
}
