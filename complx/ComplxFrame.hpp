#ifndef COMPLX_FRAME_HPP
#define COMPLX_FRAME_HPP

#include <vector>

#include "gen/ComplxFrameDecl.h"
#include "data/MemoryViewDataModel.hpp"
#include "data/RegisterPropertyGridItems.hpp"

class ComplxFrame : public ComplxFrameDecl
{
public:
    ComplxFrame();
    ~ComplxFrame();

private:
    wxObjectDataPtr<MemoryViewDataModel> memoryViewModel;
    std::vector<RegisterPropertyGridItems> registerProperties;
};


#endif
