Building on Linux
=================

Fedora
------

**Dependencies**

Install the following packages::

  sudo dnf install git gcc gdb OpenSceneGraph-devel OpenSceneGraph
  sudo dnf install libsigc++20-devel commoncpp2-devel swig
  sudo dnf install scons wxPython python-lxml SDL-devel SDL_image-devel
  sudo dnf install freealut freealut-devel libvorbis-devel
  sudo dnf install python-devel libjpeg-turbo-devel

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

  cd csp/csp/bin
  ./sim.py


Building on Windows
===================

TODO
