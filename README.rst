Building on Linux
=================

Debian
------

**Dependencies**

Install the following packages::

  sudo apt-get install cmake cmake-curses-gui libopenthreads-dev
  sudo apt-get install openscenegraph-3.4-dev swig scons libvorbis-dev
  sudo apt-get install libalut-dev python-wxgtk3.0 libjpeg62-turbo-dev
  sudo apt-get install libcommoncpp2-dev libsdl1.2-dev

Fedora
------

**Dependencies**

Install the following packages::

  sudo dnf install git cmake gcc gcc-c++ gdb OpenSceneGraph-devel OpenSceneGraph
  sudo dnf install libsigc++20-devel swig boost-devel
  sudo dnf install python3-scons python3-wxpython4 python3-lxml SDL-devel SDL_image-devel
  sudo dnf install freealut freealut-devel libvorbis-devel
  sudo dnf install python3-devel libjpeg-turbo-devel python3-ply

Building
--------

**Source code**

Execute::

  git clone https://github.com/nsmoooose/csp
  git clone https://github.com/nsmoooose/csp_data
  git clone https://github.com/nsmoooose/csp-osgal

**Terrain data**

Execute::

  cd csp/csp/data
  ln -s ../../../csp_data/terrain/

**osgal**

Note that this step needs to be reexecuted if you upgrade the OpenSceneGraph
package (which happens when you upgrade the Linux distribution).

Execute::

  cd csp-osgal
  ccmake .
  make
  sudo make install

This will install osgAL in /usr/local/lib/. Make sure that this path
is in your /etc/ld.so.conf.d/ with the following steps::

  sudo -i
  echo "/usr/local/lib" > /etc/ld.so.conf.d/local.conf
  ldconfig
  exit

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

  cd csp
  export PYTHONPATH=$(pwd)
  cd csp/bin
  ./sim.py


Building on Windows
===================

TODO
