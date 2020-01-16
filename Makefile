###########################################
# A Simple Makefile
###########################################


OUTDIR = $(shell pwd)/bin

.PHONY:all helper server xauth clean deps

all:
	mkdir -p $(OUTDIR)
	$(MAKE) OUTDIR=$(OUTDIR) -C helper 
	$(MAKE) OUTDIR=$(OUTDIR) -C server
	$(MAKE) OUTDIR=$(OUTDIR) -C xauth
	
helper:
	$(MAKE) OUTDIR=$(OUTDIR) -C helper 
server:
	$(MAKE) OUTDIR=$(OUTDIR) -C server
xauth:
	$(MAKE) OUTDIR=$(OUTDIR) -C xauth
	
deps:
	$(MAKE) -C deps
	
clean:
	$(MAKE) clean -C helper 
	$(MAKE) clean -C server
	$(MAKE) clean -C xauth
	# rm -f $(OUTDIR)/*
