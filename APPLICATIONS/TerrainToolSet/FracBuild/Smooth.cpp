// Combat Simulator Project - FracBuild - Fractal Terrain Generator
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
 * @file Smooth.cpp
 * 
 **/ 


#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "io.h"
#include "fcntl.h"
#include "SYS\STAT.H"
#include "math.h"

extern char cSourceFileName[256], cDestFileName[256], cStringBuffer[256];
extern long iSourceFileHandle, iDestFileHandle;
extern float fFactorD;

void Smooth(float fSmoothFactor)
{
	int iWidthX, iWidthY, x, y;
	float fA, fB, fC, fD, fX;

	int iFileLenght, iWidth, iHeight, iSuccess, i, current_width; 
	short sBuffer;
	float fWidth;

	iFileLenght = _filelength(iSourceFileHandle);
	fWidth = (float)sqrt((float)iFileLenght/2);			// compute terrain width from size of file
	iWidth = (int)fWidth;
	iHeight = (int)fWidth;

	for(i=0; i<(iFileLenght/2); i++)
	{
		_lseek(iSourceFileHandle, i*sizeof(short), SEEK_SET);
		iSuccess = _read(iSourceFileHandle, &sBuffer, (sizeof(short)));
//		printf("Count: %d, Value: %d\n",i,sBuffer);
	}


	iWidthX = iWidthY = (int)fWidth;
	current_width = iWidthX;

	printf("\ncopying source map to destination map.\n");
	for(y=0;y<current_width;y++)
	{
		printf("Processing row %d of %d\r",y,current_width);
		for(x=0;x<current_width;x++)
		{
//			m_pFractMap[y*fract_width*dist_factor+x*dist_factor] = 
//			m_pTerrainData->m_pHeightmap[y*current_width+x];

			_lseek(iSourceFileHandle, (y*current_width+x)*sizeof(short), SEEK_SET);
			iSuccess = _read(iSourceFileHandle, &sBuffer, sizeof(short));

			_lseek(iDestFileHandle, (y*current_width+x)*sizeof(short), SEEK_SET);
			iSuccess = _write(iDestFileHandle, &sBuffer, sizeof(short));
		}
	}


//	fHeightmap = m_pTerrainData->m_pHeightmap;

	// X is the point to smooth, B is its left, C its right neighbour
	// on the actual heightmap level
	// A is B's left neighbour, D is c's right neighbour
	// on the previous heightmap level,
	// so the index for A and D is -3;+3. -2;+2 would reference
	// values from the actual level

	// smooth every 2nd point in the heightmap, X direction
	printf("performing smoothing step in X direction.\n");
	for(y=0;y<iWidthY;y++)
	{
		printf("Processing row %d of %d\r",y,iWidthY);
		for(x=1;x<iWidthX-1;x+=2)
		{
			// calculate index for heightmap array
			i = y*iWidthX+x;

			// check if there is a valid B, if not, set B = X
			if((x-1) < 0)
			{
//				fB = fHeightmap[i];
				_lseek(iSourceFileHandle, i*sizeof(short), SEEK_SET);
				iSuccess = _read(iSourceFileHandle, &sBuffer, sizeof(short));
				fB = (float)sBuffer;
			}
			else
			{
//				fB = fHeightmap[i-1];
				_lseek(iSourceFileHandle, (i-1)*sizeof(short), SEEK_SET);
				iSuccess = _read(iSourceFileHandle, &sBuffer, sizeof(short));
				fB = (float)sBuffer;
			}

			// check if there is a valid A, if not, set A = B
			if((x-3) < 0)
				fA = fB;
			else
			{
//				fA = fHeightmap[i-3];
				_lseek(iSourceFileHandle, (i-3)*sizeof(short), SEEK_SET);
				iSuccess = _read(iSourceFileHandle, &sBuffer, sizeof(short));
				fA = (float)sBuffer;
			}

			// check if there is a valid C, if not, set C = X
			if((x+1) > (iWidthX-1))
			{
//				fC = fHeightmap[i];
				_lseek(iSourceFileHandle, i*sizeof(short), SEEK_SET);
				iSuccess = _read(iSourceFileHandle, &sBuffer, sizeof(short));
				fC = (float)sBuffer;
			}

			else
			{
//				fC = fHeightmap[i+1];
				_lseek(iSourceFileHandle, (i+1)*sizeof(short), SEEK_SET);
				iSuccess = _read(iSourceFileHandle, &sBuffer, sizeof(short));
				fC = (float)sBuffer;
			}

			// check if there is a valid D, if not, set D = C
			if((x+3) > (iWidthX-1))
				fD = fC;
			else
			{
//				fD = fHeightmap[i+3];
				_lseek(iSourceFileHandle, (i+3)*sizeof(short), SEEK_SET);
				iSuccess = _read(iSourceFileHandle, &sBuffer, sizeof(short));
				fD = (float)sBuffer;
			}

			fX = ((fB-fA)+(fC-fD))*fSmoothFactor;
			if(fX < 0.0) fX = 0.0;
//			fHeightmap[i] = ((fB+fC)/2)+fX;
			sBuffer = (short)((fB+fC)/2)+fX;
			_lseek(iDestFileHandle, i*sizeof(short), SEEK_SET);
			iSuccess = _write(iDestFileHandle, &sBuffer, sizeof(short));
		}
	}
	// smooth every 2nd point in the heightmap, Y direction
	printf("performing smoothing step in Y direction.\n");
	for(x=0; x<iWidthX; x++)
	{
		printf("Processing row %d of %d\r",x,iWidthX);
		for(y=1; y<iWidthY-1; y+=2)
		{
			// calculate index for heightmap array
			i = y*iWidthX+x;
			
			// check if there is a valid B, if not, set B = X
			if((y-1) < 0)
			{
//				fB = fHeightmap[i];
				_lseek(iSourceFileHandle, i*sizeof(short), SEEK_SET);
				iSuccess = _read(iSourceFileHandle, &sBuffer, sizeof(short));
				fB = (float)sBuffer;
			}

			else
			{
//				fB = fHeightmap[i-iWidthX];
				_lseek(iSourceFileHandle, (i-iWidthX)*sizeof(short), SEEK_SET);
				iSuccess = _read(iSourceFileHandle, &sBuffer, sizeof(short));
				fB = (float)sBuffer;
			}

			// check if there is a valid A, if not, set A = B
			if((y-3) < 0)
				fA = fB;
			else
			{
//				fA = fHeightmap[i-3*iWidthX];
				_lseek(iSourceFileHandle, (i-3*iWidthX)*sizeof(short), SEEK_SET);
				iSuccess = _read(iSourceFileHandle, &sBuffer, sizeof(short));
				fA = (float)sBuffer;
			}

			// check if there is a valid C, if not, set C = X
			if((y+1) > (iWidthY-1))
			{
//				fC = fHeightmap[i];
				_lseek(iSourceFileHandle, i*sizeof(short), SEEK_SET);
				iSuccess = _read(iSourceFileHandle, &sBuffer, sizeof(short));
				fC = (float)sBuffer;
			}
			else
			{
//				fC = fHeightmap[i+iWidthX];
				_lseek(iSourceFileHandle, (i+iWidthX)*sizeof(short), SEEK_SET);
				iSuccess = _read(iSourceFileHandle, &sBuffer, sizeof(short));
				fC = (float)sBuffer;
			}

			// check if there is a valid D, if not, set D = C
			if((y+3) > (iWidthY-1))
				fD = fC;
			else
			{
//				fD = fHeightmap[i+3*iWidthY];
				_lseek(iSourceFileHandle, (i+3*iWidthY)*sizeof(short), SEEK_SET);
				iSuccess = _read(iSourceFileHandle, &sBuffer, sizeof(short));
				fD = (float)sBuffer;
			}
		
			fX = ((fB-fA)+(fC-fD))*fSmoothFactor;
			if(fX < 0.0) fX = 0.0;
//			fHeightmap[i] = ((fB+fC)/2)+fX;
			sBuffer = (short)((fB+fC)/2)+fX;
			_lseek(iDestFileHandle, i*sizeof(short), SEEK_SET);
			iSuccess = _write(iDestFileHandle, &sBuffer, sizeof(short));
		}	
	}		
	return;
}


