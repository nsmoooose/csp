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
 * @file FracBuild.cpp
 * 
 **/ 


#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "io.h"
#include "fcntl.h"
#include "SYS\STAT.H"
#include "math.h"
#include "FracBuild.h"

char cSourceFileName[256], cDestFileName[256], cStringBuffer[256];
long iSourceFileHandle, iDestFileHandle;
float fFactorD;

/** 
 * Iterate - Implementation of the Diamond Square algorithm
 * 
 * This function reads a heightmap in 16bit RAW format and
 * writes a new map with twice the size in both directions.
 * The new points are created by interpolation and random 
 * displacement of the original map's points. 
 * The function uses direct disc access for reading and writing
 * points instead of caching them in memory. This makes it 
 * independent from system memory when processing huge maps.
 *
 * fFactorD controls the maximum deviation from the actual result 
 * of the interpolation of a group of points.
 *
 * @author Martin "Stormbringer" Hoffmann <storm_bringer@gmx.li> 
 */ 

int Iterate(float fFactorD)
{
	int iFileLenght, iWidth, iHeight, iSuccess; 
	short sBuffer;
	float fWidth;

	iFileLenght = _filelength(iSourceFileHandle);
	fWidth = (float)sqrt((float)iFileLenght/2);			// compute terrain width from size of file
	iWidth = (int)fWidth;
	iHeight = (int)fWidth;

	srand(1);

	float a, b, c, d, e, f, g, h, i, j, k;
	float highest, lowest, rnd, interval;	
	int cpos = 0, cneg = 0, x, y; 
	int fract_width, current_width, dist_factor;
	int higher = 0, _lower = 0, between = 0;

	// fract_width = width of the destination map
	fract_width = (iWidth-1) * 2 + 1;

	// dist_factor determines the spacing between height-values in dest map depending on the source/dest-resolution
	// example: 129 points source map, 1025 dest map => every 8th point in dest map comes from source map
	// resolution of dest map is determined by the number of iterations
	dist_factor = (fract_width-1) / ((fract_width-1) / 2);
	current_width = iWidth;

	printf("\nIteration process started. This may take a while.\n\n");
	printf("Filesize Source:\t%d Bytes\n", _filelength(iSourceFileHandle));

	// copy height values from source map to destination map (fract_map)
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

			_lseek(iDestFileHandle, (y*fract_width*dist_factor+x*dist_factor)*sizeof(short), SEEK_SET);
			iSuccess = _write(iDestFileHandle, &sBuffer, sizeof(short));
		}
	}

	// THE DIAMOND STEP
	printf("\nperforming diamond step.\n");
	for(y=0;y<current_width-1;y++)
	{
		printf("processing row %d of %d\r",y,current_width);
		for(x=0;x<current_width-1;x++)
		{
			_lseek(iDestFileHandle, (y*fract_width*dist_factor+x*dist_factor)*sizeof(short), SEEK_SET);
			iSuccess = _read(iDestFileHandle, &sBuffer, sizeof(short));
			a = (float)sBuffer;

			_lseek(iDestFileHandle, (y*fract_width*dist_factor+(x+1)*dist_factor)*sizeof(short), SEEK_SET);
			iSuccess = _read(iDestFileHandle, &sBuffer, sizeof(short));
			b = (float)sBuffer;

			_lseek(iDestFileHandle, ((y+1)*fract_width*dist_factor+x*dist_factor)*sizeof(short), SEEK_SET);
			iSuccess = _read(iDestFileHandle, &sBuffer, sizeof(short));
			c = (float)sBuffer;

			_lseek(iDestFileHandle, ((y+1)*fract_width*dist_factor+(x+1)*dist_factor)*sizeof(short), SEEK_SET);
			iSuccess = _read(iDestFileHandle, &sBuffer, sizeof(short));
			d = (float)sBuffer;

			// interval is the difference between the highest and the lowest corner point
			highest = max(a, max(b, max(c, d)));
			lowest = min(a, min(b, min(c, d)));
			interval = abs(highest - lowest)*fFactorD;

			// generate the random displacement value and scale it to [-interval, interval]
			rnd = rand() % (unsigned long)(interval - -interval +1) - interval;

			// calculate the average height value of the four quad corners
			// and add the random value to the point in the middle of the quad
			e = (a+b+c+d)/4 + rnd;
			if(e<0) e=0;
 
			sBuffer = (short)e;
			_lseek(iDestFileHandle, (y*fract_width*dist_factor+(fract_width*dist_factor/2)+
									 x*dist_factor+(dist_factor/2))*sizeof(short), SEEK_SET);
			iSuccess = _write(iDestFileHandle, &sBuffer, sizeof(short));

#ifdef DEBUG
			printf("a: %.2f, b: %.2f, c: %.2f, d: %.2f, e: %.2f, buf (e): %d\n", a, b, c, d, e, sBuffer);
#endif
		}
	}
	// THE SQUARE STEP
	printf("\nperforming square step.\n");
	for(y=0;y<current_width-1;y++)
	{
		printf("processing row %d of %d\r",y,current_width);
		for(x=0;x<current_width-1;x++)
		{
			_lseek(iDestFileHandle, (y*fract_width*dist_factor+x*dist_factor)*sizeof(short), SEEK_SET);
			iSuccess = _read(iDestFileHandle, &sBuffer, sizeof(short));
			a = (float)sBuffer;

			_lseek(iDestFileHandle, (y*fract_width*dist_factor+(x+1)*dist_factor)*sizeof(short), SEEK_SET);
			iSuccess = _read(iDestFileHandle, &sBuffer, sizeof(short));
			b = (float)sBuffer;

			_lseek(iDestFileHandle, ((y+1)*fract_width*dist_factor+x*dist_factor)*sizeof(short), SEEK_SET);
			iSuccess = _read(iDestFileHandle, &sBuffer, sizeof(short));
			c = (float)sBuffer;

			_lseek(iDestFileHandle, ((y+1)*fract_width*dist_factor+(x+1)*dist_factor)*sizeof(short), SEEK_SET);
			iSuccess = _read(iDestFileHandle, &sBuffer, sizeof(short));
			d = (float)sBuffer;

			_lseek(iDestFileHandle, (y*fract_width*dist_factor+(fract_width*dist_factor/2)+
									 x*dist_factor+(dist_factor/2))*sizeof(short), SEEK_SET);
			iSuccess = _read(iDestFileHandle, &sBuffer, sizeof(short));
			e = (float)sBuffer;

			// check if there's a left neighbor for f
			if(x==0)
			{
				// no!
				highest = max(a, max(c, e));
				lowest = min(a, min(c, e));
				interval = abs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;

//				f = (a+c+e)/3 + (rnd * fFactorD);
				f = (a+c+e)/3 + rnd;
#ifdef DEBUG
				printf("a: %.2f, c: %.2f, e: %.2f, f: %.2f, ", a, c, e, f);
#endif
			}
			// there is a left neighbor, so get the value!
			else
			{
				_lseek(iDestFileHandle, (y*fract_width*dist_factor+(fract_width*dist_factor/2)+
	  								     x*dist_factor-(dist_factor/2))*sizeof(short), SEEK_SET);
				iSuccess = _read(iDestFileHandle, &sBuffer, sizeof(short));
				j = (float)sBuffer;

				highest = max(a, max(c, max(e, j)));
				lowest = min(a, min(c, min(e, j)));
				interval = abs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;

				f = (a+c+e+j)/4 + rnd;
#ifdef DEBUG
				printf("a: %.2f, c: %.2f, e: %.2f, j: %.2f, f: %.2f ", a, c, e, j, f);
#endif
			}
			if(f<0) f=0;

			sBuffer = (short)f;
			_lseek(iDestFileHandle, (y*fract_width*dist_factor+(fract_width*dist_factor/2)+
									 x*dist_factor)*sizeof(short), SEEK_SET);
			iSuccess = _write(iDestFileHandle, &sBuffer, sizeof(short));
#ifdef DEBUG			
			printf("buf (f): %d\n", sBuffer);
#endif
			// check if there's an upper neighbor for g
			if(y==0)
			{
				// no!
				highest = max(a, max(b, e));
				lowest = min(a, min(b, e));
				interval = abs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;

				g = (a+b+e)/3 + rnd;
#ifdef DEBUG
				printf("a: %.2f, b: %.2f, e: %.2f, g: %.2f, ", a, b, e, g);
#endif
			}
			// there is a upper neighbor, so get the value!
			else
			{
				_lseek(iDestFileHandle, (y*fract_width*dist_factor-(fract_width*dist_factor/2)+
								         x*dist_factor+(dist_factor/2))*sizeof(short), SEEK_SET);
				iSuccess = _read(iDestFileHandle, &sBuffer, sizeof(short));
				k = (float)sBuffer;

				highest = max(a, max(b, max(e, k)));
				lowest = min(a, min(b, min(e, k)));
				interval = abs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;
				
				g = (a+b+e+k)/4 + rnd;
#ifdef DEBUG
				printf("a: %.2f, b: %.2f, e: %.2f, g: %.2f, k: %.2f, ", a, b, e, g, k);
#endif
			}
			if(g<0) g=0;

			sBuffer = (short)g;
			_lseek(iDestFileHandle, (y*fract_width*dist_factor+x*dist_factor+(dist_factor/2))*sizeof(short), SEEK_SET);
			iSuccess = _write(iDestFileHandle, &sBuffer, sizeof(short));
#ifdef DEBUG
			printf("buf (g): %d\n",sBuffer);
#endif
			// do we have to calculate i?
			if(y==current_width-2)
			{
				//yes
				highest = max(c, max(d, e));
				lowest = min(c, min(d, e));
				interval = abs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;
				
				i = (c+d+e)/3 + rnd;
				if(i<0) i=0;
#ifdef DEBUG
				printf("c: %.2f, d: %.2f, e: %.2f, i: %.2f, ", c, d, e, i);
#endif
				sBuffer = (short)i;
				_lseek(iDestFileHandle, ((y+1)*fract_width*dist_factor+x*dist_factor+(dist_factor/2))*sizeof(short), SEEK_SET);
				iSuccess = _write(iDestFileHandle, &sBuffer, sizeof(short));
#ifdef DEBUG
				printf("buf (i): %d\n", sBuffer);
#endif
			}

			// do we have to calculate h?
			if(x==current_width-2)
			{
				highest = max(b, max(d, e));
				lowest = min(b, min(d, e));
				interval = abs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;
				
				h = (b+d+e)/3 + rnd;

				if(h<0) h=0;
				sBuffer = (short)h;
				_lseek(iDestFileHandle, (y*fract_width*dist_factor+(fract_width*dist_factor/2)+
										(x+1)*dist_factor)*sizeof(short), SEEK_SET);
				iSuccess = _write(iDestFileHandle, &sBuffer, sizeof(short));
#ifdef DEBUG
				printf("b: %.2f, d: %.2f, e: %.2f, h: %.2f, buf (h): %d\n", b, d, e, h, sBuffer);
#endif
			}
		}  // end of square step
	}
	printf("\nopeartion finished.\n");
	printf("filesize destination:\t%d bytes\n", _filelength(iDestFileHandle));
	return 0;
}

void Gauss()
{

/* Iterate - Implementation of the Diamond Square algorithm
 *
 * Blurs the heightmap image with a Gaussian filter algorithm,
 * outputs to dst using a filter kernel which must be a 2D float
 * array of size [2*k+1][2*k+1] 
 * 
 * The algorithm used is based on sample code from a gamasutra-article
 */

	int k=3, width, iFileLength, iSuccess;
	float fWidth;
	short sBuffer;

	double fFilterKernel[] =
	{
		.00, .00, .00,   .05,  .00, .00, .00,
		.00, .05, .18,   .32,  .18, .05, .00,
		.00, .18, .64,  1.00,  .64, .18, .00,
		.05, .32, 1.00, 1.00, 1.00, .32, .05,
		.00, .18, .64,  1.00,  .64, .18, .00,
		.00, .05, .18,   .32,  .18, .05, .00,
		.00, .00, .00,   .05,  .00, .00, .00,
	}; 

	iFileLength = _filelength(iSourceFileHandle);
	fWidth = (float)sqrt((float)iFileLength/2);			// compute terrain width from size of file
	width = (int)fWidth;

//  the gauss algorithm itself
	for (int y=0;y<width;y++)
	{
		for (int x=0;x<width;x++)
		{
			float total=0;
			double *kp=fFilterKernel;
			int offset;

			for (int j=-k;j<=k;j++)
			for (int i=-k;i<=k;i++)
			{
				int x2=x+i;
				int y2=y+j;

				offset=y2*width+x;
				if(offset<0) offset=0;
				_lseek(iSourceFileHandle, (offset)*sizeof(short), SEEK_SET);
				iSuccess = _read(iSourceFileHandle, &sBuffer, sizeof(short));

				if (x2>=0 && x2<width && y2>=0 && y2<width) 
					total+=(float)kp[0]*sBuffer;
				kp++;
			}
			total /= 7;
			if (total<0) total=0; else if (total>60000) total=60000;
//			dst[y*width+x]=total;

			sBuffer = (short)total;
			_lseek(iDestFileHandle, (y*width+x)*sizeof(short), SEEK_SET);
			iSuccess = _write(iDestFileHandle, &sBuffer, sizeof(short));
		}
	}

	return;
}

void ConvertTo24Bit()
{
	int i, width, iFileLength, iSuccess, iNumPoints;
	float fWidth;
	short sBuffer;
	char cZero=0, cHiByte, cLoByte;

	iFileLength = _filelength(iSourceFileHandle);
	iNumPoints = iFileLength/2;
	fWidth = (float)sqrt((float)iFileLength/2);			// compute terrain width from size of file
	width = (int)fWidth;

	for(i=0;i<iNumPoints;i++)
	{
		iSuccess = _read(iSourceFileHandle, &sBuffer, sizeof(short));
		cHiByte = (char)sBuffer & 0xFF;
		cLoByte = (char)(sBuffer >> 8);
		iSuccess = _write(iDestFileHandle, &cZero, sizeof(char));
		iSuccess = _write(iDestFileHandle, &cLoByte, sizeof(char));
		iSuccess = _write(iDestFileHandle, &cHiByte, sizeof(char));
	}

	printf("Dest file width: %d\n Bytes", width);
}

int main(int argc, char* argv[])
{
	int operation, i;

	printf("CSP - Fractal Terrain Builder\n"
		   "Copyright (C) 2002 The Combat Simulator Project\n"
		   "http://csp.sourceforge.net\n\n");

	if(argc < 4)
	{
		printf("ERROR: Not enough parameters!\n\n"
			   "Usage: FracBuild sourcefile destfile operation [factor]\n\n"
			   "Operations:\n\n"
			   "-iterate\tcreates one iteration level. x and y resolution of the heightmap"
			   "\t\tare doubled. the destination file is four times larger than\n" 
			   "\t\tthe source, so be careful with large maps! `factor`\n"
			   "\t\tindicates the maximum random deviation of an interpolated\n"
			   "\t\tpoint. Example: 0.10 generates fractal points which are no more\n"
			   "\t\tthan 10%% higher or lower as the interpolated value. Keeping the\n"
			   "\t\tfactor low makes sure that the resulting fractal map keeps the\n"
			   "\t\tshape of the source map.\n\n"
			   "-s:standard\tsmoothes the map. this is necessary because fractalisation\n"
			   "\t\tsometimes causes rough edges, which make the terrain look\n"
			   "\t\tartificial. `factor` has the similar meaning as with `iterate`.\n\n"
			   "-s:gauss\tis another smoothing method, using a gaussian filter. this\n"
			   "\t\tfilter modifies not only the fractally generated points, but\n"
			   "\t\tthe original values as well. no factor parameter is required.\n\n"
			   "-to24\t\tconverts a RAW format heightmap to the 24bit format required\n"
			   "\t\tfor use with demeter. the file size will be increased, without\n"
			   "\t\timproving image quality, so it makes no sense using this\n"
			   "\t\tfunction without demeter.\n");
		return -1;
	}

	// Source and Destination file names
	strcpy(cSourceFileName, argv[1]);
	strcpy(cDestFileName, argv[2]);

	// check for other paramters
	for(i=0; i<argc-3; i++)
	{
		strcpy(cStringBuffer, argv[i+3]);

		if((stricmp(cStringBuffer, "-s:gauss"))==0)
		{
			operation = GAUSS;
			break;
		}

		if((stricmp(cStringBuffer, "-s:standard"))==0)
		{
			operation = STANDARD;
			strcpy(cStringBuffer, argv[i+4]);
			fFactorD = (float)atof(cStringBuffer);
			break;
		}

		if((stricmp(cStringBuffer, "-to24"))==0)
		{
			operation = TO24;
			break;
		}

		if((stricmp(cStringBuffer, "-iterate"))==0)
		{
			operation = ITERATE;
			strcpy(cStringBuffer, argv[i+4]);
			fFactorD = (float)atof(cStringBuffer);
			break;
		}
	}

	iSourceFileHandle = _open(cSourceFileName, _O_BINARY);
	if(iSourceFileHandle==-1)
	{	
		printf("ERROR: unable to open '%s'!\n", cSourceFileName);
		char text[100];
		gets(text);
		return -1;
	}

	iDestFileHandle = _open(cDestFileName, _O_CREAT | _O_RDWR | _O_BINARY, _S_IREAD | _S_IWRITE);
	if(iDestFileHandle==-1)
	{	
		printf("ERROR: unable to open '%s'!\n", cDestFileName);
		char text[100];
		gets(text);
		_close(iSourceFileHandle);
		return -1;
	}


	switch(operation)
	{
	case ITERATE:
		Iterate(fFactorD);
		break;

	case GAUSS:
		Gauss();
		break;

	case STANDARD:
		Smooth(fFactorD);
		break;

	case TO24:
		ConvertTo24Bit();
		break;
	}

	_close(iSourceFileHandle);
	_close(iDestFileHandle);

	return 0;
}

