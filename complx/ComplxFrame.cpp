#include "ComplxFrame.hpp"

ComplxFrame::ComplxFrame() : ComplxFrameDecl(nullptr), memoryViewModel(new MemoryViewDataModel())
{
    memoryView->AssociateModel(memoryViewModel.get());
    memoryView->AppendColumn(new wxDataViewColumn("", new wxDataViewTextRenderer(), MemoryInfo, 32));
    memoryView->AppendColumn(new wxDataViewColumn("Address", new wxDataViewTextRenderer(), MemoryAddress));
    memoryView->AppendColumn(new wxDataViewColumn("Hexadecimal", new wxDataViewTextRenderer(), MemoryHexadecimal));
    memoryView->AppendColumn(new wxDataViewColumn("Decimal", new wxDataViewTextRenderer(), MemoryDecimal));
    memoryView->AppendColumn(new wxDataViewColumn("Binary", new wxDataViewTextRenderer(), MemoryBinary));
    memoryView->AppendColumn(new wxDataViewColumn("Label", new wxDataViewTextRenderer(), MemoryLabel));
    memoryView->AppendColumn(new wxDataViewColumn("Instruction", new wxDataViewTextRenderer(), MemoryInstruction));
    memoryView->AppendColumn(new wxDataViewColumn("Comment", new wxDataViewTextRenderer(), MemoryComment));

    for (unsigned int i = 0; i < 8; i++)
    {
        RegisterPropertyGridItems property(wxString::Format("R%d", i), i * 1000);
        property.Add(statePropGrid);
        registerProperties.push_back(property);
    }
}

ComplxFrame::~ComplxFrame()
{

}
