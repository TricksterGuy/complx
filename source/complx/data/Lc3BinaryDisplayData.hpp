#ifndef LC3_BINARY_DISPLAY_DATA
#define LC3_BINARY_DISPLAY_DATA

#include <functional>
#include <list>
#include <optional>

#include <lc3.hpp>

#include <wx/object.h>
#include <wx/string.h>
#include <wx/variant.h>

class Lc3BinaryDisplayData : public wxObject
{
public:
    Lc3BinaryDisplayData() : instruction(0), binary('0', 16) {}
    Lc3BinaryDisplayData(uint16_t instr, const wxString& bin, const std::list<RLEColorEntry> c) : instruction(instr), binary(bin), colors(c) {}
    ~Lc3BinaryDisplayData() override = default;
    Lc3BinaryDisplayData& operator=(const Lc3BinaryDisplayData& other) = default;
    bool operator==(const Lc3BinaryDisplayData& other) const;
    bool operator!=(const Lc3BinaryDisplayData& other) const { return !(*this == other); }
    uint16_t instruction;
    wxString binary;
    std::list<RLEColorEntry> colors;

    DECLARE_DYNAMIC_CLASS(Lc3BinaryDisplayData)
};
DECLARE_VARIANT_OBJECT(Lc3BinaryDisplayData)

Lc3BinaryDisplayData ConstructBinaryDisplayData(uint16_t instruction, const InstructionPlugin* instruction_plugin);



#endif
