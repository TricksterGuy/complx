#ifndef MEMORY_VIEW_DATA_MODEL_HPP
#define MEMORY_VIEW_DATA_MODEL_HPP

#include <lc3_all.hpp>

#include <wx/dataview.h>

enum
{
    MemoryInfo = 0,
    MemoryAddress,
    MemoryHexadecimal,
    MemoryDecimal,
    MemoryBinary,
    MemoryLabel,
    MemoryInstruction,
    MemoryComment,
    MemorySize,
};

class MemoryViewDataModel : public wxDataViewVirtualListModel
{
public:
    MemoryViewDataModel(std::reference_wrapper<lc3_state> state, unsigned int disassemble_level = LC3_BASIC_DISASSEMBLE);
    ~MemoryViewDataModel() {}

    unsigned int GetColumnCount() const override { return MemorySize; }
    unsigned int GetCount() const override { return 0x10000; }
    wxString GetColumnType(unsigned int col) const override;
    void GetValueByRow(wxVariant& variant, unsigned int row, unsigned int col) const override;
    bool SetValueByRow(const wxVariant& variant, unsigned int row, unsigned int col) override;

private:
    std::reference_wrapper<lc3_state> state_ref;
    unsigned int disassemble_level;
};


#endif
