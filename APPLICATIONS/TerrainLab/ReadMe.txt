///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////


TerrainLab shows a demo for a fractal terrain generation algorithm. It is a modified version of the "diamond-square"- algorithm presented in http://www.lighthouse3d.com/opengl/terrain/ (here it is called "midpoint-displacement"). it enables us to create terrain heightmaps of virtually unlimited resolution based upon satellite-DEM-data, which is only available in a 1km x 1km resolution (at least for free). this algo adds random "erosion" to a heightmap, taking into consideration the geological structure of the terrain. only one paramter is needed: FactorD. It defines the maximum amount of random displacement allowed per iterarion, depending of the surrounding height points. The application reduces FactorD after each iteration, which causes smaller displacement in the following iteration.

There are several ways to create terrain from a given heightmap:
- Use small values (0.1 - 0.3) for FactorD in the first 2 or 3 iterations. This will increase the resolution of the map by doubling the number of vertices per step, but it won't change the general appearance of the terrain. With such little numbers the biggest amount of a new heightmap point comes from interpolating its neighboring values. In the next step, set FactorD to 0.6-1.0 to add erosion.

- Apply a series of larger values (0.8, 0.7, 0.6) to add a lot of noise to the terrain, then reduce FactorD to 0.1-0.3 in the following steps. This will "calm down" the map a little.

- As the final step, always apply terrain smoothing. This will reduce the "spikes" which are generated in the steps before. If you are not satisfied with the result, apply it two or more times.

use this image to understand the implementation given in "CFractal::DiamondSquare":

variable names representing heightmap points per quad


		k
		|
		|
		|
	a-------g-------b
	|	|	|
	|	|	|
	|	|	|
j-------f-------e-------h-------l
	|	|	|
	|	|	|
	|	|	|
	c-------i-------d
		|
		|
		|
		m

key commands are:

a,y 		- accelerate/deccelerate
+,- (numpad)	- iterate/de-iterate
*   (numpad)	- perform smoothing step
8,2 (numpad)	- factorD
F1		- load mesh dialog

turn camera with the mouse + left button