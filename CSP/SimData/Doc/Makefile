DATESTR := $(shell date +'%Y%m%d')

default: all

all:
	@echo 'generating doxygen documentation...'
	@doxygen Doxyfile >doxygen.log 2>doxygen.err

dist:
	ln -s html simdata-docs-$(DATESTR)
	tar vczhf simdata-docs-$(DATESTR).tgz simdata-docs-$(DATESTR)
	rm -f simdata-docs-$(DATESTR)

