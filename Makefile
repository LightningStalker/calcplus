# It builds all programs in the current directory.

SUFFIX   := cpp

BASENAME := basename
ECHO     := /bin/echo
RM       := rm
TEST     := /bin/test
XARGS    := xargs

#CXXFLAGS    := -Wall
BASENAMEOPT := -a -s .$(SUFFIX) -z
ECHOPT      := -e -n
ERRORMSG    := "Nothing to do!!"
XARGSOPT    := -0 -I {}

FILES     := $(wildcard *.$(SUFFIX))
NOSUFFIX  := $(BASENAME) $(BASENAMEOPT) $(FILES)
XARGSXE   := $(XARGS) $(XARGSOPT)
ECHOXE    := $(ECHO) $(ECHOPT)

STUFF     != for F in *.$(SUFFIX); do \
    $(TEST) "$$($(BASENAME) -a -s .$(SUFFIX) $$F)" -ot "$$F" && \
    $(ECHOXE) "$$F "; done
    
NOSTUFFIX := $(BASENAME) $(BASENAMEOPT) $(STUFF)

all: .cpp

unconditional:
	$(NOSUFFIX) | $(XARGSXE) $(CXX) $(CXXFLAGS) -o {} {}.$(SUFFIX)

.cpp:
	@if [ -z "$(STUFF)" ]; then echo $(ERRORMSG); exit 1; fi
	$(NOSTUFFIX) | $(XARGSXE) $(CXX) $(CXXFLAGS) -o {} {}.$(SUFFIX)

clean:
	$(NOSUFFIX) | $(XARGSXE) $(RM) {}
