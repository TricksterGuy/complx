#ifndef MEMORYVIEW_HPP
#define MEMORYVIEW_HPP

#include <set>
#include <wx/string.h>
#include <wx/grid.h>
#include <wx/imaglist.h>
#include "lc3.hpp"

extern lc3_state state;

enum class ViewAction {
  HIDE = 0,
  SHOW = 1,
  SHOW_MODDED = 2,
};

class ViewRange {
  public:
    unsigned short NumElements() const {return end - start + 1;}

  private:
    unsigned short start;
    unsigned short end;
    ViewAction action;
};

enum
{
    MemoryInfo = 0,
    MemoryAddress,
    MemoryHexadecimal,
    MemoryDecimal,
    MemoryBinary,
    MemoryLabel,
    MemoryInstruction,
    MemorySize,
};

class MemoryView : public wxGridTableBase
{
	public:
		MemoryView();
		~MemoryView();
    int GetNumberRows();
    int GetNumberCols();
    wxString GetValue(int row, int col);
    void SetValue(int row, int col, const wxString &value);
    wxString GetColLabelValue(int col);
    long GetValueAsLong(int row, int col);
    void SetDisassembleLevel(int level);
    void SetUnsignedMode(bool mode);
  private:
    int disassemble_level;
    bool unsigned_mode;
    std::set<ViewRange> ranges;
    ViewAction defaultView;
};

#endif
