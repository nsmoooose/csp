TOPDIR = ..

SOURCES = \
	NetRandom \
	NetworkInterface \
	NetworkNode \
	PeerInfo \
	RecordCodec

MESSAGES = \
	IndexMessages \
	ObjectUpdate \
	TestMessage


include $(TOPDIR)/make/standard.mk


test: test.o libSimNet.a libSimNetMessages.so
	g++ -pg -o $@ $^ $(CXXFLAGS) $(SIMDATA_LIB) $(SIMNET_LIB) \
	    -lpthread -ldl -lccgnu2 -Wl,-R. -lSimNetMessages

ShadowTest: ShadowTest.o libSimNet.a libSimNetMessages.so
	g++ -pg -o $@ $^ $(CXXFLAGS) $(SIMDATA_LIB) $(SIMNET_LIB) \
	    -lpthread -ldl -lccgnu2 -Wl,-R. -lSimNetMessages


libSimNetMessages.so: $(MSGOBJS)
	g++ -pg -shared -o $@ $^ -Wl,-z,lazyload $(SIMDATA_LIB)

libSimNet.a: $(OBJECTS)
	echo $(OBJECTS)
	ar cru $@ $^
	ranlib $@

all: libSimNet.a libSimNetMessages.so

