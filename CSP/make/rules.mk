# specialty build rules

%.cpp %.h: %.net
	../SimData/Tools/TaggedRecordCompiler/trc.py $<

%_wrap.cpp %.py: %.i
	$(SWIG) $(SWOPTS) -o $@ $<

