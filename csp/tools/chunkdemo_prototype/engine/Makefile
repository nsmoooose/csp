# Makefile for tu-testbed/engine library


TOP = ..
include $(TOP)/config

ifeq ($(TU_TESTBED_COMPILER), gcc)
SDL_CFLAGS := $(shell sdl-config --cflags)
SDL_LDFLAGS := $(shell sdl-config --libs)
endif

SDL_INCLUDE := SDL
LUA_INCLUDE := $(TOP)/../lua-4.0/include

INCLUDE_PATHS := -I$(SDL_INCLUDE) -I$(LUA_INCLUDE) -I$(ENGINE_INCLUDE) -I$(LIBPNG_INCLUDE) -I$(ZLIB_INCLUDE) -I$(JPEGLIB_INCLUDE)

CFLAGS := $(CFLAGS) $(SDL_CFLAGS) $(INCLUDE_PATHS) $(CC_DEBUG_FLAGS)
LDFLAGS := -lGL -lGLU -lSDL_image $(SDL_LDFLAGS) $(LIB_DEBUG_FLAGS)
# -lstdc++

# Source files.
SRCS := \
	utility.cpp	\
	geometry.cpp \
	cull.cpp \
	bsp.cpp \
	ogl.cpp \
	jpeg.cpp \
	image.cpp \
	image_filters.cpp \
	tqt.cpp
	#dlmalloc.c	
	#config.cpp 
OBJS := $(SRCS:.cpp=.$(OBJ_EXT))
OBJS := $(OBJS:.c=.$(OBJ_EXT))


ENGINE = engine.$(LIB_EXT)

all: $(ENGINE)

$(ENGINE): $(OBJS)
	$(AR) $(LIB_OUT_FLAG)$(ENGINE) $(OBJS)

clean:
	-@rm -f $(ENGINE) $(OBJS)
	
