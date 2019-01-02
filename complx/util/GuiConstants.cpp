#include "GuiConstants.hpp"

GuiConstants::GuiConstants()
{

#ifdef _WIN32

    offset_map = {
        // Width of the Info Column in MemoryView.
        {"INFO_COLUMN_WIDTH", 38},
        // Amount to add to the Binary Column to display all of the text centered.
        {"BINARY_COLUMN_ADDITION", 23}
    };

#elif __linux__

    offset_map = {
        {"INFO_COLUMN_WIDTH", 37},
        {"BINARY_COLUMN_ADDITION", 5}
    };

#elif __APPLE__

    /// TODO find out proper values for these guys
    /// Defaulting to linux values...
    offset_map = {
        {"INFO_COLUMN_WIDTH", 37}
        {"BINARY_COLUMN_ADDITION", 5}
    };
#endif

}
