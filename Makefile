#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX, Christopher Arndt, and Patrick Desaulniers
#

SHELL=/bin/bash

-include dpf/Makefile.base.mk

all: libs plugins gen

# --------------------------------------------------------------

PLUGINS = \
	MIDICCMapX4 \
	MIDICCRecorder \
	MIDICCToPressure \
	MIDIPBToCC \
	MIDIPressureToCC \
	MIDISysFilter

DPF_PATCHES = \
	dpf/lv2-port-groups.patch \
	dpf/fix-lv2-version-export.patch \
	dpf/no-port-name-lv2-prefix.patch

submodules:
	-test -d .git && git submodule update --init --recursive

libs: submodules patch

patch: submodules
	@-for p in $(DPF_PATCHES); do \
		echo "Applying patch '$${p}'..."; \
		patch -d dpf -r - -p1 -N -i ../patches/$${p}; \
	done

plugins: $(PLUGINS)

$(PLUGINS):
	$(MAKE) all -C plugins/$@

ifneq ($(CROSS_COMPILING),true)
gen: plugins dpf/utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh
ifeq ($(MACOS),true)
	@$(CURDIR)/dpf/utils/generate-vst-bundles.sh
endif
dpf/utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator
else
gen: plugins dpf/utils/lv2_ttl_generator.exe
	@$(CURDIR)/dpf/utils/generate-ttl.sh

dpf/utils/lv2_ttl_generator.exe:
	$(MAKE) -C dpf/utils/lv2-ttl-generator WINDOWS=true
endif

# --------------------------------------------------------------

clean:
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator
	@for plug in $(PLUGINS); do \
		$(MAKE) clean -C plugins/$${plug}; \
	done
	rm -rf bin build

install: all
	@for plug in $(PLUGINS); do \
		$(MAKE) install -C plugins/$${plug}; \
	done

install-user: all
	@for plug in $(PLUGINS); do \
		$(MAKE) install-user -C plugins/$${plug}; \
	done

# --------------------------------------------------------------

.PHONY: all clean gen install install-user libs patch plugins submodules
