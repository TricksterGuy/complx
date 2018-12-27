#ifndef MEMORY_VIEW_DATA_MODEL_HPP
#define MEMORY_VIEW_DATA_MODEL_HPP

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
    MemoryViewDataModel() : wxDataViewVirtualListModel(0x10000) {}
    ~MemoryViewDataModel() {}

    unsigned int GetColumnCount() const override { return MemorySize; }
    unsigned int GetCount() const override { return 0x10000; }
    wxString GetColumnType(unsigned int col) const override;
    void GetValueByRow(wxVariant& variant, unsigned int row, unsigned int col) const override;
    bool SetValueByRow(const wxVariant& variant, unsigned int row, unsigned int col) override;

private:
};


#endif
