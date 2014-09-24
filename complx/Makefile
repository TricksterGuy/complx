# File intended only to be ran with GNU make. Use any other make program at your own risk!
OUTPUT := complx
WXCPPFLAGS := $(shell wx-config --cppflags)
WXCPPLIBS := $(shell wx-config --libs)
CPPFLAGS := $(MASTER_CPP_FLAGS) -Wall -I../liblc3 -I. -DPREFIX=$(PREFIX) $(WXCPPFLAGS)
LDFLAGS := $(MASTER_CPP_FLAGS) -Wall -rdynamic -L../liblc3
LDLIBS := -llc3 $(WXCPPLIBS)
CC := g++ $(ARCH_FLAGS)
CXX := $(CC)
RM := rm
INSTALL := install
FILES := AddressDialog.o AddressDialogDecl.o BreakpointDialog.o BreakpointDialogDecl.o CallStackDialog.o CallStackDialogDecl.o ComplxApp.o ComplxFrame.o ComplxFrameDecl.o DebugInfoDialog.o DebugInfoDialogDecl.o DebugInfoList.o GridCellBinaryRenderer.o GridCellInfoRenderer.o LC3Console.o LC3ConsoleDecl.o LC3RunThread.o MemoryGrid.o MemoryView.o MemoryViewFrame.o MemoryViewFrameDecl.o RunForDialog.o RunForDialogDecl.o RunTestDialog.o RunTestDialogDecl.o WatchpointDialog.o WatchpointDialogDecl.o XmlTestParser.o

$(OUTPUT) : $(FILES)
	$(CC) $(LDFLAGS) $^ -o $(OUTPUT) $(LDLIBS)

clean : 
	$(RM) -rf *.o $(OUTPUT)

install : $(OUTPUT)
	$(INSTALL) -m 0755 $(OUTPUT) $(PREFIX)/bin

uninstall :
	$(RM) -rf $(PREFIX)/bin/$(OUTPUT)
