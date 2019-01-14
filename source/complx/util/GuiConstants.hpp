#ifndef GUI_CONSTANTS_HPP
#define GUI_CONSTANTS_HPP

#include <unordered_map>
#include <wx/colour.h>
#include <wx/gdicmn.h>

class GuiConstants
{
public:
    GuiConstants(const GuiConstants&) = delete;
    GuiConstants& operator=(const GuiConstants) = delete;
    int GetOffset(const std::string& key) const { return offset_map.at(key); }
    const wxSize& GetSize(const std::string& key) const { return size_map.at(key); }
    const std::string& GetString(const std::string& key) const { return string_map.at(key); }
    bool HasColor(const std::string& key) const { return color_map.find(key) != color_map.end(); }
    const wxColour& GetColor(const std::string& key) const { return color_map.at(key); }
    static const GuiConstants& Instance()
    {
        static GuiConstants instance;
        return instance;
    }
private:
    GuiConstants();
    std::unordered_map<std::string, int> offset_map;
    std::unordered_map<std::string, wxSize> size_map;
    std::unordered_map<std::string, std::string> string_map;
    std::unordered_map<std::string, wxColour> color_map;
};

inline const GuiConstants& GuiConstants() {
    return GuiConstants::Instance();
}

#endif
