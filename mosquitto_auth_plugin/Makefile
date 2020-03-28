###########################################
# A Simple Makefile
###########################################

NMARCH	=$(subst _,-,$(shell uname -m))

#CXX		= g++
CXXFLAGS= -g -Wall -Wno-char-subscripts -march=$(NMARCH) -fpermissive -fPIC

NAME	= libmosquitto_auth_plugin.so
LIBNAME = libmosquitto_auth_plugin.lib
OUTDIR	?= .

CONDFLAGS =

INCLUDES:= -I. -I.. -I../helper -I../helper/jsoncpp -I/usr/local/include
LINKLIBS:= -lpthread -L../helper -lhelper

COMPILEFLAGS:=$(CXXFLAGS) $(CONDFLAGS)
LINKFLAGS:= -shared

ALLDIRS	= $(shell find . -name '*' -type d)
DIRFILES= $(wildcard $(dir)/*.cpp)
ALLFILES:= $(foreach dir,$(ALLDIRS),$(DIRFILES))

SOURCES	:= $(ALLFILES)
OBJECTS := $(SOURCES:.cpp=.o)

.PHONY:all clean

all:$(NAME) $(LIBNAME)
$(NAME): $(OBJECTS)
	$(CXX) $(LINKFLAGS) -o $@ $^ $(LINKLIBS)
	chmod a+x $@
	cp $@ $(OUTDIR)
	
$(LIBNAME): $(OBJECTS)
	ar -rcs $@ $^

$(OBJECTS):%.o:%.cpp
	$(CXX) $(COMPILEFLAGS) $(INCLUDES) -c $^ -o $@
	
clean:
	rm -f $(NAME) $(OBJECTS)
