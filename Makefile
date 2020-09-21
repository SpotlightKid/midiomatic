#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX, Christopher Arndt, and Patrick Desaulniers
#

include dpf/Makefile.base.mk

all: libs plugins gen

# --------------------------------------------------------------

submodules:
	-test -d .git && git submodule update --init --recursive

libs: submodules patch

patch:
	-patch -d dpf -r - -p1 -N -i ../patches/dpf/no-port-name-lv2-prefix.patch
	-patch -d dpf -r - -p1 -N -i ../patches/dpf/fix-lv2-version-export.patch

plugins: libs
	$(MAKE) all -C plugins/MIDICCRecorder
	$(MAKE) all -C plugins/MIDIPBToCC
	$(MAKE) all -C plugins/MIDIPressureToCC
	$(MAKE) all -C plugins/MIDISysFilter

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
	$(MAKE) clean -C plugins/MIDICCRecorder
	$(MAKE) clean -C plugins/MIDIPBToCC
	$(MAKE) clean -C plugins/MIDIPressureToCC
	$(MAKE) clean -C plugins/MIDISysFilter
	rm -rf bin build

install: all
	$(MAKE) install -C plugins/MIDICCRecorder
	$(MAKE) install -C plugins/MIDIPBToCC
	$(MAKE) install -C plugins/MIDIPressureToCC
	$(MAKE) install -C plugins/MIDISysFilter

install-user: all
	$(MAKE) install-user -C plugins/MIDICCRecorder
	$(MAKE) install-user -C plugins/MIDIPBToCC
	$(MAKE) install-user -C plugins/MIDIPressureToCC
	$(MAKE) install-user -C plugins/MIDISysFilter

# --------------------------------------------------------------

.PHONY: all clean gen install install-user libs patch plugins submodule
