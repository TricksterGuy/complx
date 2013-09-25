# File intended only to be ran with GNU make. Use any other make program at your own risk!
WXCPPFLAGS := $(shell wx-config --cppflags)
WXCPPLIBS := $(shell wx-config --libs)
CPPFLAGS := -fPIC -Wall -O3 -I../liblc3 -I. $(WXCPPFLAGS)
LDFLAGS = -fPIC -O3 -Wall -shared -L../liblc3 -Wl,-soname,$@
LDLIBS := -llc3 $(WXCPPLIBS)
CC := g++ $(ARCH_FLAGS)
CXX := $(CC)
RM := rm
INSTALL := install

all: lc3_random.so lc3_second_timer.so lc3_timer.so lc3_multiply.so lc3_bwlcd.so lc3_colorlcd.so lc3_udiv.so

lc3_random.so : random.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

lc3_second_timer.so : second_timer.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

lc3_timer.so : timer.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

lc3_multiply.so : multiply.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

lc3_bwlcd.so : bwlcd.o BWLCDGUI.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

lc3_colorlcd.so : colorlcd.o COLORLCDGUI.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

lc3_udiv.so : udiv.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean : 
	$(RM) -rf *.o *.so

install : lc3_random.so lc3_second_timer.so lc3_timer.so lc3_multiply.so lc3_bwlcd.so lc3_colorlcd.so lc3_udiv.so
	$(INSTALL) -m 0755 lc3_random.so $(PREFIX)/lib/lc3_random.so
	$(INSTALL) -m 0755 lc3_second_timer.so $(PREFIX)/lib/lc3_second_timer.so
	$(INSTALL) -m 0755 lc3_timer.so $(PREFIX)/lib/lc3_timer.so
	$(INSTALL) -m 0755 lc3_multiply.so $(PREFIX)/lib/lc3_multiply.so
	$(INSTALL) -m 0755 lc3_bwlcd.so $(PREFIX)/lib/lc3_bwlcd.so
	$(INSTALL) -m 0755 lc3_colorlcd.so $(PREFIX)/lib/lc3_colorlcd.so
	$(INSTALL) -m 0755 lc3_udiv.so $(PREFIX)/lib/lc3_udiv.so

uninstall :
	$(RM) -rf $(PREFIX)/lib/lc3_random.so
	$(RM) -rf $(PREFIX)/lib/lc3_second_timer.so
	$(RM) -rf $(PREFIX)/lib/lc3_timer.so
	$(RM) -rf $(PREFIX)/lib/lc3_multiply.so
	$(RM) -rf $(PREFIX)/lib/lc3_bwlcd.so
	$(RM) -rf $(PREFIX)/lib/lc3_colorlcd.so
	$(RM) -rf $(PREFIX)/lib/lc3_udiv.so
