This will be the Ground Combat Demo project (Win32 only for the time being)

If someone wants to participate, just tell me :)

The Demo will use the Auran Jet engine, which is free for non-commercial use and can be obtained at http://www.auran.com/jet (you must fill out a form to download...)

In this demo we will also try to work out the CSP Object Specification and hopefully the organisation of CSP *content* .

Step 1: Create a basic game skeleton -- Done.
Step 2: Create required classes (Input, OnScreenDisplay, Sound, Lighting)
Step 3: Work out Entity Classes based on the CSP object specs (Entity = Object in world)

I'll not post the sources yet, since i want to iron out some unnecessarily complicated stuff from the basic setup to keep it as easy to handle as possible.


Features planned for version 1.0 :

- Terrain loading (not sure if i can use our Terrain engine yet, but would love to)
- Sky rendering including fog (look at /Test/Marve/gwain to get an impression)
- basic user controlled First Person Camera (mouse-look and walking keys)
- simple shooting system (aka WeaponSubSystem(1))
- basic 3d object placing routines (to put some stuff onto the terrain)

Features planned for the near future:

- Placement of random vegetation on the terrain including animated (!!) Trees and grass (see 3DMark2001->Demo)
- Precise Collision Detection for normal Objects
- Implementation of Buildings (incl. their collision detection, lighting)
- Basic networking implementation based on the CSP rules (which are available by then hopefully)

Features planned for the future:

- Particle system use (smoke, fire, explosion effects)
- Creating a debris system for use in combination with the Particles (more realistic explosions)
- Modifying the scene entities to make them vulnerable to explosions and projectile impacts
- Combined complex explosion effect 
  (with:
	fire, 
	smoke, 
	debris, 
	heatblast (PixelShader maybe?), 
	terrain deformation(?) and 
	(partial) destruction of buildings)

- Realtime shadow casting by all entities (if performantly possible, i vote for static shadows if entity is immobile)
- Changing Sky depending on world time (Sun, Moon+Stars, Clouds (?))
- Lens Flare effects for certain types of ammo (such as signal flares), Sun and Moon, vehicle headlights, streetlights
  and the like

- Dynamic weather system (maybe even based on realworld data?) including Rain, Thunderstorm, Snowfall etc.

<New_Idea>
- Fade-Over-Time system:
  A subsystem that makes objects in the scenery appear rotten over time (destroyed buildings & vehicles for example)
  Possible solution: Add an "Age" property to an entity which will load different mesh/textures depending on it.
</New_Idea>


that's about all i got in mind for this at the moment (maybe more to come).





GOALS:
- Make that thing work basically ;-)
- Try to get all features working >>flawlessly<<
- Refine visual details to have a top-of-the-edge graphics result


(1) Means, we have to create classes for the Weapon itself plus it's projectiles. I got some ideas for that already.