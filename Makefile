# File intended only to be ran with GNU make. Use any other make program at your own risk!
PREFIX := /usr/local
export PREFIX
MASTER_CPP_FLAGS := -O3 -s -std=c++11
#MASTER_CPP_FLAGS := -g -std=c++11
export MASTER_CPP_FLAGS
INSTALL := install
ARCH = $(shell getconf LONG_BIT)
export ARCH

all: liblc3.so as2obj lc3runner lc3test lc3plugins complx

liblc3.so :
	$(MAKE) -C liblc3 liblc3.so -f Makefile

as2obj : liblc3.so
	$(MAKE) -C $@ $@ -f Makefile

#comp : liblc3.so
#	$(MAKE) -C $@ $@ -f Makefile
	
complx : liblc3.so
	$(MAKE) -C $@ $@ -f Makefile

lc3runner : liblc3.so
	$(MAKE) -C $@ $@ -f Makefile

lc3test : liblc3.so
	$(MAKE) -C $@ $@ -f Makefile

lc3plugins : liblc3.so
	$(MAKE) -C $@ all -f Makefile

clean :
	$(MAKE) -C liblc3 clean -f Makefile
	$(MAKE) -C as2obj clean -f Makefile
	$(MAKE) -C lc3runner clean -f Makefile
	$(MAKE) -C lc3test clean -f Makefile
	$(MAKE) -C lc3plugins clean -f Makefile
	$(MAKE) -C complx clean -f Makefile
#	$(MAKE) -C comp clean -f Makefile

install : all
	mkdir -p $(PREFIX)
	mkdir -p $(PREFIX)/bin
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/share/complx-tools
	mkdir -p $(PREFIX)/share/icons
	mkdir -p $(PREFIX)/share/applications
	$(MAKE) -C liblc3 install -f Makefile
	$(MAKE) -C as2obj install -f Makefile
	$(MAKE) -C lc3runner install -f Makefile
	$(MAKE) -C lc3test install -f Makefile
	$(MAKE) -C lc3plugins install -f Makefile
	$(MAKE) -C complx install -f Makefile
#	$(MAKE) -C comp install -f Makefile
	$(INSTALL) -m 0644 complx-tools.pdf $(PREFIX)/share/complx-tools
	$(INSTALL) -m 0644 ComplxChangeLog.txt $(PREFIX)/share/complx-tools
	$(INSTALL) -m 0644 ChangeLog $(PREFIX)/share/complx-tools
	$(INSTALL) -m 0644 PattPatelAppA.pdf $(PREFIX)/share/complx-tools
	$(INSTALL) -m 0644 complx.png $(PREFIX)/share/icons
	$(INSTALL) -m 0644 complx.desktop $(PREFIX)/share/applications
	$(INSTALL) -m 0644 complx-tips.txt $(PREFIX)/share/complx-tools
	@echo remember to run sudo ldconfig

uninstall :
	$(MAKE) -C liblc3 uninstall -f Makefile
	$(MAKE) -C as2obj uninstall -f Makefile
	$(MAKE) -C lc3runner uninstall -f Makefile
	$(MAKE) -C lc3test uninstall -f Makefile
	$(MAKE) -C lc3plugins uninstall -f Makefile
	$(MAKE) -C complx uninstall -f Makefile
#	$(MAKE) -C comp uninstall -f Makefile

