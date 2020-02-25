###########################################
# A Simple Makefile
###########################################

NMARCH	=$(subst _,-,$(shell uname -m))

#CXX		= g++
CXXFLAGS= -g -Wall -Wno-deprecated -march=$(NMARCH) -fpermissive

APPNAME	= xauth
OUTDIR	?= .

CONDFLAGS = 

INCLUDES:= -I. -I.. -I../helper -I../helper/jsoncpp -I$(PGHOME)/include -I/usr/local/include
LINKLIBS:= -lrt -lssl -lcrypto -lpthread -lhiredis -L$(PGHOME)/lib -lpq -L../helper -lhelper -L../server -lserver

COMPILEFLAGS:=$(CXXFLAGS) $(CONDFLAGS)
LINKFLAGS:= -Wl,-rpath=.

ALLDIRS	= $(shell find . -name '*' -type d)
DIRFILES= $(wildcard $(dir)/*.cpp)
ALLFILES:= $(foreach dir,$(ALLDIRS),$(DIRFILES))

SOURCES	:= $(ALLFILES)
OBJECTS := $(SOURCES:.cpp=.o)

.PHONY:all clean

all:$(APPNAME)
$(APPNAME): $(OBJECTS)
	$(CXX) $(LINKFLAGS) -o $@ $^ $(LINKLIBS)
	chmod a+x $@
	cp $@ $(OUTDIR)

$(OBJECTS):%.o:%.cpp
	$(CXX) $(COMPILEFLAGS) $(INCLUDES) -c $^ -o $@
	
clean:
	rm -f $(APPNAME) $(OBJECTS)
