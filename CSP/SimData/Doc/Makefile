DATESTR := $(shell date +'%Y%m%d')

default: all

all:
	doxygen Doxyfile

dist:
	ln -s html simdata-docs-$(DATESTR)
	tar vczhf simdata-docs-$(DATESTR).tgz simdata-docs-$(DATESTR)
	rm -f simdata-docs-$(DATESTR)

