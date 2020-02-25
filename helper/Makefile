###########################################
# A Simple Makefile
###########################################

NMARCH	=$(subst _,-,$(shell uname -m))

#CXX		= g++
CXXFLAGS= -g -Wall -Wno-char-subscripts -march=$(NMARCH) -fpermissive -fPIC

NAME	= libhelper.so
LIBNAME = libhelper.lib
OUTDIR	?= .

CONDFLAGS =

INCLUDES:= -I. -I./jsoncpp
LINKLIBS:= -lpthread -lcurl -lssl -luuid

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
