# Spatial Index Library
#
# Copyright (C) 2002  Navel Ltd.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Contact information:
#  Mailing address:
#    Marios Hadjieleftheriou
#    University of California, Riverside
#    Department of Computer Science
#    Surge Building, Room 310
#    Riverside, CA 92521
#
#  Email:
#    marioh@cs.ucr.edu

DATE    = `date +%y%m%d%H%M`
BACKUP  = ..
LIBTARGET = libspatialindex.a
VERSION = 062b

CXX = g++
CXXFLAGS = -O1 -DNDEBUG -fPIC

OBJECTS =\
	spatialindex/Point.o spatialindex/Region.o \
	spatialindex/TimePoint.o spatialindex/TimeRegion.o \
	spatialindex/SpatialIndexImpl.o storagemanager/StorageManager.o \
	storagemanager/MemoryStorageManager.o \
	storagemanager/DiskStorageManager.o \
	storagemanager/Buffer.o \
	storagemanager/RandomEvictionsBuffer.o \
	rtree/Statistics.o rtree/Node.o rtree/Index.o rtree/Leaf.o \
	rtree/RTree.o \
	quadtree/QuadTree.o \

all: libspatialindex.a 

debug:
	$(MAKE) -C spatialindex debug
	$(MAKE) -C storagemanager debug
	$(MAKE) -C rtree debug
	$(MAKE) -C quadtree debug
	ar rcv $(LIBTARGET) $(OBJECTS)
	ranlib $(LIBTARGET)

libspatialindex.a: src
	ar rcv $(LIBTARGET) $(OBJECTS)
	ranlib $(LIBTARGET) 

libspatialindex.so: src
	ld -shared -soname lib/libspatialindex.so.1 -o lib/libspatialindex.so.1.0 -lc $(OBJECTS)

src:
	$(MAKE) -C spatialindex
	$(MAKE) -C storagemanager
	$(MAKE) -C rtree 
	$(MAKE) -C quadtree 

clean:
	$(MAKE) -C spatialindex clean
	$(MAKE) -C storagemanager clean
	$(MAKE) -C rtree clean
	$(MAKE) -C quadtree clean
	rm -rf $(LIBTARGET)

backup:
	tar cjf $(BACKUP)/spatialindex.$(VERSION).tar.bz2 -C .. --exclude RCS spatialindex
