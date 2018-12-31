#ifndef MEMORY_VIEW_BINARY_DATA_RENDERER_HPP
#define MEMORY_VIEW_BINARY_DATA_RENDERER_HPP

#include <wx/dataview.h>

class MemoryViewBinaryDataRenderer : public wxDataViewCustomRenderer
{
public:
    MemoryViewBinaryDataRenderer();
    ~MemoryViewBinaryDataRenderer() {}

    bool Render(wxRect cell, wxDC *dc, int state) override;

protected:
private:
};


#endif

