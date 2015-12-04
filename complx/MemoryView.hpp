#ifndef MEMORYVIEW_HPP
#define MEMORYVIEW_HPP

#include <map>
#include <set>
#include <vector>
#include <wx/string.h>
#include <wx/grid.h>
#include <wx/imaglist.h>

enum class ViewAction {
  HIDE = 0,
  SHOW = 1,
  SHOW_MODDED = 2,
};

// Represents a user defined memory address range with action as to whether it is viewable or not.
struct ViewRange {
    ViewRange(unsigned short _start, unsigned short _end, ViewAction _action = ViewAction::SHOW) :
       start(_start), end(_end), action(_action) {}
    unsigned short NumElements() const {return end - start + 1;}
    unsigned short start;
    unsigned short end;
    ViewAction action;
};

// Comparator that gives priority to larger ranges
struct ViewRangeElementCompare {
    bool operator()(const ViewRange& lhs, const ViewRange& rhs)
    {
        return lhs.NumElements() > rhs.NumElements();
    }
};

// Class to map id's in view table to memory address for MemoryView
struct ViewTableEntry {
  ViewTableEntry(int _id, unsigned short _address) : id(_id), address(_address) {}
  const int id;
  const unsigned short address;
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
		MemoryView() : disassemble_level(2), unsigned_mode(false), flipped_mode(false) {}
		void ShowAllAddresses();
		void ModifyAddresses(const std::vector<ViewRange>& addresses, bool expand_too = true);
		void SetDefaultVisibility(ViewAction action);
        int GetNumberRows();
        int GetNumberCols();
        wxString GetValue(int row, int col);
        void SetValue(int row, int col, const wxString &value);
        wxString GetColLabelValue(int col);
        long GetValueAsLong(int row, int col);
        void SetDisassembleLevel(int level);
        void SetUnsignedMode(bool mode);
        void SetFlippedMode(bool mode);
        // Converts address to view table id
        // If not in the viewTable returns closest id.
        int AddressToView(unsigned short address) const;
    private:
        int disassemble_level;
        bool unsigned_mode;
        bool flipped_mode;
        void ExpandRanges();
        unsigned short GetAddress(int index);
        std::set<ViewRange, ViewRangeElementCompare> ranges;
        // Expanded version of ranges.
        std::vector<ViewTableEntry> viewTable;
        std::map<unsigned short, int> viewTable_rev;
        ViewAction defaultView;
};

#endif
