///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////
//
//   Class CFractal
//	 Implementation of the modified Diamond Square Algorithm
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	09/25/2001	file created		-	Stormbringer
//	09/27/2001  smooth step added, roughness in DiamondSquare
//				removed				-	Stormbringer
//
///////////////////////////////////////////////////////////////////////////

#include "Fractal.h"

CFractal::CFractal()
{
	srand(1);
}

CFractal::~CFractal()
{
}

// bilinear interpolation routine
float CFractal::MiddleRandomPoint(float a, float b, float c, float d, float rnd)
{
	float u = 0.5 + rnd, v = 0.5 + rnd;
	float m = (1 - v) * ((1-u)*a + u*b) + v * ((1-u)*c + u*d);
	return m;
}

void CFractal::DiamondSquare(float fFactorD)
{
	float a, b, c, d, e, f, g, h, i, j, k;
	float highest, lowest, rnd, interval;	
	int count, cpos = 0, cneg = 0, x, y; 
	int fract_width, current_width, dist_factor;
	int higher = 0, _lower = 0, between = 0;

	// fract_width = width of the destination map
	fract_width = (m_pTerrainData->m_iMeshWidth-1) * 2 + 1;
	m_pFractMap = new float[fract_width*fract_width];

	// clear the map
	for(count=0;count<fract_width*fract_width;count++)
		m_pFractMap[count] = 0;

	// dist_factor determines the spacing between height-values in dest map depending on the source/dest-resolution
	// example: 129 points source map, 1025 dest map => every 8th point in dest map comes from source map
	// resolution of dest map is determined by the number of iterations
	dist_factor = (fract_width-1) / ((fract_width-1) / 2);
	current_width = m_pTerrainData->m_iMeshWidth;

	// copy height values from source map to destination map (fract_map)
	for(y=0;y<current_width;y++)
	{
		for(x=0;x<current_width;x++)
		{
			m_pFractMap[y*fract_width*dist_factor+x*dist_factor] = 
			m_pTerrainData->m_pHeightmap[y*current_width+x];
		}
	}
	delete(m_pTerrainData->m_pHeightmap);

	// THE DIAMOND STEP
	for(y=0;y<current_width-1;y++)
		for(x=0;x<current_width-1;x++)
		{
			a = m_pFractMap[y*fract_width*dist_factor+x*dist_factor];
			b = m_pFractMap[y*fract_width*dist_factor+(x+1)*dist_factor];
			c = m_pFractMap[(y+1)*fract_width*dist_factor+x*dist_factor];
			d = m_pFractMap[(y+1)*fract_width*dist_factor+(x+1)*dist_factor];

			// interval is the difference between the highest and the lowest corner point
			highest = max(a, max(b, max(c, d)));
			lowest = min(a, min(b, min(c, d)));
			interval = fabs(highest - lowest)*fFactorD;

			// generate the random displacement value and scale it to [-interval, interval]
			rnd = (rand() % (unsigned long)(interval - -interval +1)) - interval;

			// calculate the average height value of the four quad corners
			// and add the random value to the point in the middle of the quad
//			e = (a+b+c+d)/4 + (rnd * fFactorD);
//			e = (a+b+c+d)/4 + rnd;

			// check for division by zero, and call the bilinear interpolation function
			if(interval != 0)
				e = MiddleRandomPoint(a, b, c, d, rnd / (4.0*interval));
			else
				e = a;

			if(e<0) e=0;
 			m_pFractMap[y*fract_width*dist_factor+(fract_width*dist_factor/2)+
					    x*dist_factor+(dist_factor/2)] = e;
		}

	// THE SQUARE STEP
	for(y=0;y<current_width-1;y++)
		for(x=0;x<current_width-1;x++)
		{
			a = m_pFractMap[y*fract_width*dist_factor+x*dist_factor];
			b = m_pFractMap[y*fract_width*dist_factor+(x+1)*dist_factor];
			c = m_pFractMap[(y+1)*fract_width*dist_factor+x*dist_factor];
			d = m_pFractMap[(y+1)*fract_width*dist_factor+(x+1)*dist_factor];
			e = m_pFractMap[y*fract_width*dist_factor+(fract_width*dist_factor/2)+
						    x*dist_factor+(dist_factor/2)];

			// check if there's a left neighbor for f
			if(x==0)
			{
				// no!
				highest = max(a, max(c, e));
				lowest = min(a, min(c, e));
				interval = fabs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;

//				f = (a+c+e)/3 + (rnd * fFactorD);
//				f = (a+c+e)/3 + rnd;
				if(interval != 0)
					f = MiddleRandomPoint(a, c, e, a, rnd / (4.0*interval));
				else
					f = a;
			}
			// there is a left neighbor, so get the value!
			else
			{
				j = m_pFractMap[y*fract_width*dist_factor+(fract_width*dist_factor/2)+
							    x*dist_factor-(dist_factor/2)];

				highest = max(a, max(c, max(e, j)));
				lowest = min(a, min(c, min(e, j)));
				interval = fabs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;

//				f = (a+c+e+j)/4 + rnd;
				if(interval != 0)
					f = MiddleRandomPoint(a, c, e, j, rnd / (4.0*interval));
				else
					f = a;
			}
			if(f<0) f=0;
			m_pFractMap[y*fract_width*dist_factor+(fract_width*dist_factor/2)+
					    x*dist_factor] = f;

			// check if there's an upper neighbor for g
			if(y==0)
			{
				// no!
				highest = max(a, max(b, e));
				lowest = min(a, min(b, e));
				interval = fabs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;

//				g = (a+b+e)/3 + rnd;
				if(interval != 0)
					g = MiddleRandomPoint(a, b, e, b, rnd / (4.0*interval));
				else
					g = a;
			}
			// there is a upper neighbor, so get the value!
			else
			{
				k = m_pFractMap[y*fract_width*dist_factor-(fract_width*dist_factor/2)+
						        x*dist_factor+(dist_factor/2)];

				highest = max(a, max(b, max(e, k)));
				lowest = min(a, min(b, min(e, k)));
				interval = fabs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;
				
//				g = (a+b+e+k)/4 + rnd;
				if(interval != 0)
					g = MiddleRandomPoint(a, b, e, k, rnd / (4.0*interval));
				else
					g = a;
			}
			if(g<0) g=0;
			m_pFractMap[y*fract_width*dist_factor+x*dist_factor+(dist_factor/2)] = g;

			// do we have to calculate i?
			if(y==current_width-2)
			{
				//yes
				highest = max(c, max(d, e));
				lowest = min(c, min(d, e));
				interval = fabs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;
				
//				i = (c+d+e)/3 + rnd;
				if(interval != 0)
					i = MiddleRandomPoint(c, d, e, e, rnd / (4.0*interval));
				else	
					i = c;
			}
			if(i<0) i=0;
			m_pFractMap[(y+1)*fract_width*dist_factor+x*dist_factor+(dist_factor/2)] = i;

			// do we have to calculate h?
			if(x==current_width-2)
			{
				highest = max(b, max(d, e));
				lowest = min(b, min(d, e));
				interval = fabs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;
				
//				h = (b+d+e)/3 + rnd;
				if(interval != 0)
					h = MiddleRandomPoint(b, d, e, b, rnd / (4.0*interval));
				else
					h = b;

				if(h<0) h=0;
				m_pFractMap[y*fract_width*dist_factor+(fract_width*dist_factor/2)+
						   (x+1)*dist_factor] = h;
			}
		}  // end of square step

	// delete the temporary fractal Heightmap, copy its values to the main mesh before
	m_pTerrainData->m_pHeightmap = new float[fract_width*fract_width];
	for(count=0; count < fract_width*fract_width; count++)
		m_pTerrainData->m_pHeightmap[count] = (float)m_pFractMap[count];
	delete m_pFractMap;

	// set the new terrain width in m_pTerrainData
	m_pTerrainData->m_fFactorD = fFactorD;
	
	return;
}

void CFractal::DiamondSquareAlt(float fFactorD)
{
	float a, b, c, d, e, f, g, h, i, j, k;
	float highest, lowest, rnd, interval;	
	int count, cpos = 0, cneg = 0, x, y; 
	int fract_width, current_width, dist_factor;
	int higher = 0, _lower = 0, between = 0;

	// fract_width = width of the destination map
	fract_width = (m_pTerrainData->m_iMeshWidth-1) * 2 + 1;
	m_pFractMap = new float[fract_width*fract_width];

	// clear the map
	for(count=0;count<fract_width*fract_width;count++)
		m_pFractMap[count] = 0;

	// dist_factor determines the spacing between height-values in dest map depending on the source/dest-resolution
	// example: 129 points source map, 1025 dest map => every 8th point in dest map comes from source map
	// resolution of dest map is determined by the number of iterations
	dist_factor = (fract_width-1) / ((fract_width-1) / 2);
	current_width = m_pTerrainData->m_iMeshWidth;

	// copy height values from source map to destination map (fract_map)
	for(y=0;y<current_width;y++)
	{
		for(x=0;x<current_width;x++)
		{
			m_pFractMap[y*fract_width*dist_factor+x*dist_factor] = 
			m_pTerrainData->m_pHeightmap[y*current_width+x];
		}
	}
	delete(m_pTerrainData->m_pHeightmap);

	// THE DIAMOND STEP
	for(y=0;y<current_width-1;y++)
		for(x=0;x<current_width-1;x++)
		{
			a = m_pFractMap[y*fract_width*dist_factor+x*dist_factor];
			b = m_pFractMap[y*fract_width*dist_factor+(x+1)*dist_factor];
			c = m_pFractMap[(y+1)*fract_width*dist_factor+x*dist_factor];
			d = m_pFractMap[(y+1)*fract_width*dist_factor+(x+1)*dist_factor];

			// interval is the difference between the highest and the lowest corner point
			highest = max(a, max(b, max(c, d)));
			lowest = min(a, min(b, min(c, d)));
			interval = fabs(highest - lowest)*fFactorD;

			// generate the random displacement value and scale it to [-interval, interval]
			rnd = (rand() % (unsigned long)(interval - -interval +1)) - interval;

			// calculate the average height value of the four quad corners
			// and add the random value to the point in the middle of the quad
//			e = (a+b+c+d)/4 + (rnd * fFactorD);
			e = (a+b+c+d)/4 + rnd;

			if(e<0) e=0;
 
			m_pFractMap[y*fract_width*dist_factor+(fract_width*dist_factor/2)+
					    x*dist_factor+(dist_factor/2)] = e;
		}

	// THE SQUARE STEP
	for(y=0;y<current_width-1;y++)
		for(x=0;x<current_width-1;x++)
		{
			a = m_pFractMap[y*fract_width*dist_factor+x*dist_factor];
			b = m_pFractMap[y*fract_width*dist_factor+(x+1)*dist_factor];
			c = m_pFractMap[(y+1)*fract_width*dist_factor+x*dist_factor];
			d = m_pFractMap[(y+1)*fract_width*dist_factor+(x+1)*dist_factor];
			e = m_pFractMap[y*fract_width*dist_factor+(fract_width*dist_factor/2)+
						    x*dist_factor+(dist_factor/2)];

			// check if there's a left neighbor for f
			if(x==0)
			{
				// no!
				highest = max(a, max(c, e));
				lowest = min(a, min(c, e));
				interval = fabs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;

//				f = (a+c+e)/3 + (rnd * fFactorD);
				f = (a+c+e)/3 + rnd;
			}
			// there is a left neighbor, so get the value!
			else
			{
				j = m_pFractMap[y*fract_width*dist_factor+(fract_width*dist_factor/2)+
							    x*dist_factor-(dist_factor/2)];

				highest = max(a, max(c, max(e, j)));
				lowest = min(a, min(c, min(e, j)));
				interval = fabs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;

				f = (a+c+e+j)/4 + rnd;
			}
			if(f<0) f=0;
			m_pFractMap[y*fract_width*dist_factor+(fract_width*dist_factor/2)+
					    x*dist_factor] = f;

			// check if there's an upper neighbor for g
			if(y==0)
			{
				// no!
				highest = max(a, max(b, e));
				lowest = min(a, min(b, e));
				interval = fabs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;

				g = (a+b+e)/3 + rnd;
			}
			// there is a upper neighbor, so get the value!
			else
			{
				k = m_pFractMap[y*fract_width*dist_factor-(fract_width*dist_factor/2)+
						        x*dist_factor+(dist_factor/2)];

				highest = max(a, max(b, max(e, k)));
				lowest = min(a, min(b, min(e, k)));
				interval = fabs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;
				
				g = (a+b+e+k)/4 + rnd;
			}
			if(g<0) g=0;
			m_pFractMap[y*fract_width*dist_factor+x*dist_factor+(dist_factor/2)] = g;

			// do we have to calculate i?
			if(y==current_width-2)
			{
				//yes
				highest = max(c, max(d, e));
				lowest = min(c, min(d, e));
				interval = fabs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;
				
				i = (c+d+e)/3 + rnd;
			}
			if(i<0) i=0;
			m_pFractMap[(y+1)*fract_width*dist_factor+x*dist_factor+(dist_factor/2)] = i;

			// do we have to calculate h?
			if(x==current_width-2)
			{
				highest = max(b, max(d, e));
				lowest = min(b, min(d, e));
				interval = fabs(highest - lowest)*fFactorD;
				rnd = rand() % (unsigned long)(interval - -interval +1) - interval;
				
				h = (b+d+e)/3 + rnd;

				if(h<0) h=0;
				m_pFractMap[y*fract_width*dist_factor+(fract_width*dist_factor/2)+
						   (x+1)*dist_factor] = h;
			}
		}  // end of square step

	// delete the temporary fractal Heightmap, copy its values to the main mesh before
	m_pTerrainData->m_pHeightmap = new float[fract_width*fract_width];
	for(count=0; count < fract_width*fract_width; count++)
		m_pTerrainData->m_pHeightmap[count] = (float)m_pFractMap[count];
	delete m_pFractMap;

	// set the new terrain width in m_pTerrainData
	m_pTerrainData->m_fFactorD = fFactorD;
	
	return;
}

// SMOOTH STEP
// We use this step to finally smooth out any "spikes" which are generated
// by the Diamond Square algorithm. The smoothing step has to be applied
// at the end of the heighmap generation process, but it may be called at any
// step before as well. It is possible to call it more that once per step if the
// result is not satisfying.

void CFractal::SmoothStep(float fSmoothFactor)
{
	int iWidthX, iWidthY, x, y, i;
	float *fHeightmap;
	float fDiffAB, fDiffAC, fA, fB, fC;

	iWidthX = iWidthY = m_pTerrainData->m_iMeshWidth;
	fHeightmap = m_pTerrainData->m_pHeightmap;

	// first run: x++
	for(y=0; y<iWidthY; y++)
		for(x=0; x<iWidthX-2; x++)
		{
			i = y * iWidthX + x;
			fA = fHeightmap[i];
			fB = fHeightmap[i+1];
			fC = fHeightmap[i+2];

			fDiffAB = (float)fabs(fA - fB);
			fDiffAC = (float)fabs(fA - fC);

			if(fDiffAB > fDiffAC)
			{
				fB = (fA + fC) / 2;
				fHeightmap[i+1] = fB;
			}
		}

	// second run: x--
	for(y=0; y<iWidthY; y++)
		for(x=iWidthX-1; x>1; x--)
		{
			i = y * iWidthX + x;
			fA = fHeightmap[i];
			fB = fHeightmap[i-1];
			fC = fHeightmap[i-2];

			fDiffAB = (float)fabs(fA - fB);
			fDiffAC = (float)fabs(fA - fC);

			if(fDiffAB > fDiffAC)
			{
				fB = (fA + fC) / 2;
				fHeightmap[i-1] = fB;
			}
		}

	// third run: y++
	for(x=0; x<iWidthX; x++)
		for(y=0; y<iWidthY-2; y++)
		{
			i = y * iWidthX +x;
			fA = fHeightmap[i];
			fB = fHeightmap[i+iWidthX];
			fC = fHeightmap[i+(2*iWidthX)];

			fDiffAB = (float)fabs(fA - fB);
			fDiffAC = (float)fabs(fA - fC);

			if(fDiffAB > fDiffAC)
			{
				fB = (fA + fC) / 2;
				fHeightmap[i+iWidthX] = fB;
			}
		}

	// fourth run: y--
	for(x=0; x<iWidthX; x++)
		for(y=iWidthY-1; y>1; y--)
		{
			i = y * iWidthX +x;
			fA = fHeightmap[i];
			fB = fHeightmap[i-iWidthX];
			fC = fHeightmap[i-(2*iWidthX)];

			fDiffAB = (float)fabs(fA - fB);
			fDiffAC = (float)fabs(fA - fC);

			if(fDiffAB > fDiffAC)
			{
				fB = (fA + fC) / 2;
				fHeightmap[i-iWidthX] = fB;
			}
		}

	return;
}


void CFractal::SmoothStepAlternative(float fSmoothFactor)
{
	int iWidthX, iWidthY, x, y;
	float fA, fB, fC, fD, fX;
	float *fHeightmap;

	int iFileLenght, iWidth, iHeight, iSuccess, i, current_width; 
	short sBuffer;
	float fWidth;

	fHeightmap = m_pTerrainData->m_pHeightmap;
	iWidthX = iWidthY = m_pTerrainData->m_iMeshWidth;

	// X is the point to smooth, B is its left, C its right neighbour
	// on the actual heightmap level
	// A is B's left neighbour, D is c's right neighbour
	// on the previous heightmap level,
	// so the index for A and D is -3;+3. -2;+2 would reference
	// values from the actual level

	// smooth every 2nd point in the heightmap, X direction
	for(y=0;y<iWidthY;y++)
	{
		for(x=1;x<iWidthX-1;x+=1)
		{
			// calculate index for heightmap array
			i = y*iWidthX+x;

			// check if there is a valid B, if not, set B = X
			if((x-1) < 0)
			{
				fB = fHeightmap[i];
			}
			else
			{
				fB = fHeightmap[i-1];
			}

			// check if there is a valid A, if not, set A = B
			if((x-3) < 0)
				fA = fB;
			else
			{
				fA = fHeightmap[i-3];
			}

			// check if there is a valid C, if not, set C = X
			if((x+1) > (iWidthX-1))
			{
				fC = fHeightmap[i];
			}

			else
			{
				fC = fHeightmap[i+1];
			}

			// check if there is a valid D, if not, set D = C
			if((x+3) > (iWidthX-1))
				fD = fC;
			else
			{
				fD = fHeightmap[i+3];
			}

			fX = ((fB-fA)+(fC-fD))*fSmoothFactor;
//			if(fX < 0.0) fX = 0.0;
			fHeightmap[i] = ((fB+fC)/2)+fX;
		}
	}
	// smooth every 2nd point in the heightmap, Y direction
	for(x=0; x<iWidthX; x++)
	{
		for(y=1; y<iWidthY-1; y+=1)
		{
			// calculate index for heightmap array
			i = y*iWidthX+x;
			
			// check if there is a valid B, if not, set B = X
			if((y-1) < 0)
			{
				fB = fHeightmap[i];
			}

			else
			{
				fB = fHeightmap[i-iWidthX];
			}

			// check if there is a valid A, if not, set A = B
			if((y-3) < 0)
				fA = fB;
			else
			{
				fA = fHeightmap[i-3*iWidthX];
			}

			// check if there is a valid C, if not, set C = X
			if((y+1) > (iWidthY-1))
			{
				fC = fHeightmap[i];
			}
			else
			{
				fC = fHeightmap[i+iWidthX];
			}

			// check if there is a valid D, if not, set D = C
			if((y+3) > (iWidthY-1))
				fD = fC;
			else
			{
				fD = fHeightmap[i+3*iWidthY];
			}
		
			fX = ((fB-fA)+(fC-fD))*fSmoothFactor;
			if(fX < 0.0) fX = 0.0;
			fHeightmap[i] = ((fB+fC)/2)+fX;
		}	
	}		
	return;
}

