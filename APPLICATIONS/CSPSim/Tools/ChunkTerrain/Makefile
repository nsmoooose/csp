# Makefile for chunklod programs


TOP = .
include $(TOP)/config

INCLUDE_PATHS := -I$(SDL_INCLUDE) -I$(LUA_INCLUDE) -I$(ENGINE_INCLUDE) -I$(LIBPNG_INCLUDE) -I$(ZLIB_INCLUDE) -I$(JPEGLIB_INCLUDE)

CFLAGS := $(CFLAGS) $(SDL_CFLAGS) $(INCLUDE_PATHS) $(CC_DEBUG_FLAGS)
LDFLAGS := $(SDL_LDFLAGS) $(LIB_DEBUG_FLAGS) $(LDFLAGS)

%.$(OBJ_EXT) : %.cpp
	$(CC) -c $^ $(CFLAGS)

all: \
	make_engine	\
	heightfield_chunker$(EXE_EXT)	\
	heightfield_shader$(EXE_EXT)	\
	shader2$(EXE_EXT)	\
	merge-ppm$(EXE_EXT)     \
	makebt$(EXE_EXT)	\
	maketqt$(EXE_EXT)	\
	decimate_texture$(EXE_EXT)

heightfield_chunker$(EXE_EXT): heightfield_chunker.$(OBJ_EXT) mmap_util.$(OBJ_EXT) bt_array.$(OBJ_EXT) $(ENGINE)
	$(CC) -o $@ $^ $(JPEGLIB) $(LIBS) $(LDFLAGS)

heightfield_shader$(EXE_EXT): heightfield_shader.$(OBJ_EXT) mmap_util.$(OBJ_EXT) bt_array.$(OBJ_EXT) $(ENGINE)
	$(CC) -o $@ $^ $(JPEGLIB) $(LIBS) $(LDFLAGS)

shader2$(EXE_EXT): shader2.$(OBJ_EXT) mmap_util.$(OBJ_EXT) bt_array.$(OBJ_EXT) $(ENGINE)
	$(CC) -o $@ $^ $(JPEGLIB) $(LIBS) $(LDFLAGS)

merge-ppm$(EXE_EXT): merge-ppm.$(OBJ_EXT) mmap_util.$(OBJ_EXT) bt_array.$(OBJ_EXT) $(ENGINE)
	$(CC) -o $@ $^ $(JPEGLIB) $(LIBS) $(LDFLAGS)

makebt$(EXE_EXT): makebt.$(OBJ_EXT) $(ENGINE)
	$(CC) -o $@ $^ $(LIBPNG) $(ZLIB) $(LIBS) $(LDFLAGS)

maketqt$(EXE_EXT): maketqt.$(OBJ_EXT) $(ENGINE)
	$(CC) -o $@ $^ $(JPEGLIB) $(LIBS) $(LDFLAGS)

decimate_texture$(EXE_EXT): decimate_texture.$(OBJ_EXT) $(ENGINE)
	$(CC) -o $@ $^ $(JPEGLIB) $(LIBS) $(LDFLAGS)

make_engine:
	make -C engine

clean:
	make -C engine clean
	-@rm -f *.$(OBJ_EXT)
	-@rm -f heightfield_chunker$(EXE_EXT) heightfield_shader$(EXE_EXT) makebt$(EXE_EXT)
	-@rm -f shader2$(EXE_EXT) merge-ppm$(EXE_EXT) maketqt$(EXE_EXT) decimate_texture$(EXE_EXT)

