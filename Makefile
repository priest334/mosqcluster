###########################################
# A Simple Makefile
###########################################


OUTDIR = $(shell pwd)/bin

.PHONY:all helper server xauth clean deps mosquitto

all:
	mkdir -p $(OUTDIR)
	$(MAKE) OUTDIR=$(OUTDIR) -C helper 
	$(MAKE) OUTDIR=$(OUTDIR) -C server
	$(MAKE) OUTDIR=$(OUTDIR) -C xauth
	$(MAKE) OUTDIR=$(OUTDIR) -C mosquitto_auth_plugin
	$(MAKE) OUTDIR=$(OUTDIR) -C mosquitto_auth_service
	$(MAKE) OUTDIR=$(OUTDIR) -C mosquitto_cluster_plugin
	
helper:
	$(MAKE) OUTDIR=$(OUTDIR) -C helper 
server:
	$(MAKE) OUTDIR=$(OUTDIR) -C server
xauth:
	$(MAKE) OUTDIR=$(OUTDIR) -C xauth
mosquitto: mosquitto_auth_plugin mosquitto_auth_service mosquitto_cluster_plugin
mosquitto_auth_plugin:	
	$(MAKE) OUTDIR=$(OUTDIR) -C mosquitto_auth_plugin
mosquitto_auth_service:	
	$(MAKE) OUTDIR=$(OUTDIR) -C mosquitto_auth_service
mosquitto_cluster_plugin:	
	$(MAKE) OUTDIR=$(OUTDIR) -C mosquitto_cluster_plugin
	
deps:
	$(MAKE) -C deps
	
clean:
	$(MAKE) clean -C helper 
	$(MAKE) clean -C server
	$(MAKE) clean -C xauth
	$(MAKE) clean -C mosquitto_auth_plugin
	$(MAKE) clean -C mosquitto_auth_service
	$(MAKE) clean -C mosquitto_cluster_plugin
	# rm -f $(OUTDIR)/*
