#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX, Christopher Arndt, and Patrick Desaulniers
#

include dpf/Makefile.base.mk

all: libs plugins gen

# --------------------------------------------------------------

submodules:
	git submodule update --init --recursive

libs:

plugins: libs
	$(MAKE) all -C plugins/MIDISysFilter
	$(MAKE) all -C plugins/MIDIPBToCC

ifneq ($(CROSS_COMPILING),true)
gen: plugins dpf/utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh
ifeq ($(MACOS),true)
	@$(CURDIR)/dpf/utils/generate-vst-bundles.sh
endif

dpf/utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator
else
gen:
endif

# --------------------------------------------------------------

clean:
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator
	$(MAKE) clean -C plugins/MIDISysFilter
	$(MAKE) clean -C plugins/MIDIPBToCC
	rm -rf bin build

install: all
	$(MAKE) install -C plugins/MIDISysFilter
	$(MAKE) install -C plugins/MIDIPBToCC

install-user: all
	$(MAKE) install-user -C plugins/MIDISysFilter
	$(MAKE) install-user -C plugins/MIDIPBToCC

# --------------------------------------------------------------

.PHONY: all clean install install-user plugins submodule
