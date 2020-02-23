#!/usr/bin/make -f
# Breton Master Makefile
#
# Magnus Larsen
# Feb 2020

all: desktop # playstation

desktop:
	$(MAKE) -f desktop.mk

playstation:
	$(MAKE) -f playstation.mk

clean:
	$(MAKE) -f desktop.mk clean
	$(MAKE) -f playstation.mk clean

.PHONY: all desktop playstation clean