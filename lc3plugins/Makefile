# File intended only to be ran with GNU make. Use any other make program at your own risk!
WXCPPFLAGS := $(shell wx-config --cppflags)
WXCPPLIBS := $(shell wx-config --libs)
CPPFLAGS := $(MASTER_CPP_FLAGS) -fPIC -Wall -I../liblc3 -I. $(WXCPPFLAGS)
LDFLAGS = $(MASTER_CPP_FLAGS) -fPIC -Wall -shared -L../liblc3 -Wl,-soname,$@
LDLIBS := -llc3 $(WXCPPLIBS)
CC := g++ $(ARCH_FLAGS)
CXX := $(CC)
RM := rm
INSTALL := install

all: liblc3_random.so liblc3_second_timer.so liblc3_timer.so liblc3_multiply.so liblc3_bwlcd.so liblc3_colorlcd.so liblc3_udiv.so

liblc3_random.so : random.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

liblc3_second_timer.so : second_timer.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

liblc3_timer.so : timer.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

liblc3_multiply.so : multiply.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

liblc3_bwlcd.so : bwlcd.o BWLCDGUI.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

liblc3_colorlcd.so : colorlcd.o COLORLCDGUI.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

liblc3_udiv.so : udiv.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean : 
	$(RM) -rf *.o *.so

install : liblc3_random.so liblc3_second_timer.so liblc3_timer.so liblc3_multiply.so liblc3_bwlcd.so liblc3_colorlcd.so liblc3_udiv.so
	$(INSTALL) -m 0755 liblc3_random.so $(PREFIX)/lib/liblc3_random.so
	$(INSTALL) -m 0755 liblc3_second_timer.so $(PREFIX)/lib/liblc3_second_timer.so
	$(INSTALL) -m 0755 liblc3_timer.so $(PREFIX)/lib/liblc3_timer.so
	$(INSTALL) -m 0755 liblc3_multiply.so $(PREFIX)/lib/liblc3_multiply.so
	$(INSTALL) -m 0755 liblc3_bwlcd.so $(PREFIX)/lib/liblc3_bwlcd.so
	$(INSTALL) -m 0755 liblc3_colorlcd.so $(PREFIX)/lib/liblc3_colorlcd.so
	$(INSTALL) -m 0755 liblc3_udiv.so $(PREFIX)/lib/liblc3_udiv.so

uninstall :
	$(RM) -rf $(PREFIX)/lib/liblc3_random.so
	$(RM) -rf $(PREFIX)/lib/liblc3_second_timer.so
	$(RM) -rf $(PREFIX)/lib/liblc3_timer.so
	$(RM) -rf $(PREFIX)/lib/liblc3_multiply.so
	$(RM) -rf $(PREFIX)/lib/liblc3_bwlcd.so
	$(RM) -rf $(PREFIX)/lib/liblc3_colorlcd.so
	$(RM) -rf $(PREFIX)/lib/liblc3_udiv.so
