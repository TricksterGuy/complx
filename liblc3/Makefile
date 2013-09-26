# File intended only to be ran with GNU make. Use any other make program at your own risk!
OUTPUT := liblc3.so
CPPFLAGS := $(MASTER_CPP_FLAGS) -fPIC -Wall -DLINUX
LDFLAGS := $(MASTER_CPP_FLAGS) -fPIC -Wall -shared -Wl,-soname,$(OUTPUT) -L.
LDLIBS := -lstdc++ -ldl #-llc3_priv$(ARCH)
CC := g++ $(ARCH_FLAGS)
CXX := $(CC)
RM := rm
INSTALL := install
FILES := ExpressionEvaluator.o lc3_cmp.o lc3_debug.o lc3_expressions.o lc3_params.o lc3_plugin.o lc3_symbol.o lc3_test.o

$(OUTPUT) : $(FILES)
	$(CC) $(LDFLAGS) $^ -o $(OUTPUT) $(LDLIBS)

clean : 
	$(RM) -rf $(FILES) $(OUTPUT)

install : $(OUTPUT)
	$(INSTALL) -m 0755 $(OUTPUT) $(PREFIX)/lib

uninstall :
	$(RM) -rf $(PREFIX)/lib/$(OUTPUT)
