Building on Linux
=================

Fedora
------

**Dependencies**

Install the following packages::

  sudo dnf install git gcc gdb OpenSceneGraph-devel OpenSceneGraph
  sudo dnf libsigc++20-devel commoncpp2-devel swig
  sudo dnf scons wxPython python-lxml SDL-devel SDL_image-devel
  sudo dnf freealut freealut-devel libvorbis-devel
  sudo dnf python-devel libjpeg-turbo-devel

**Source code**

Execute::

  git clone https://github.com/nsmoooose/csp
  git clone https://github.com/nsmoooose/csp_data
  git clone https://github.com/nsmoooose/csp-osgal

**Terrain data**

Execute::

  cd csp/csp/data
  ln -s ../../../csp_data/terrain/

**Compiling**

Execute the following commands::

  cd csp

  # Make it possible for all the python scripts to be found
  # correctly when building and running the simulator.
  export PYTHONPATH=$(pwd)

  cd csp

  # This is needed the first time in order to check all
  # dependencies.
  scons config

  # Build the simulator.
  scons all

**Running**

Execute the following::

  cd csp/csp/bin
  ./sim.py


Building on Windows
===================

TODO
