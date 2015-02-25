// Scintilla platform layer for wxWidgets

#ifndef PLATWX_H
#define PLATWX_H

#include "Platform.h"

#include <wx/colour.h>
#include <wx/gdicmn.h> // wxRect

class wxDC;

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

inline wxColour wxColourFromCDandAlpha(ColourDesired cd, int alpha)
{
    return wxColour((unsigned char)cd.GetRed(),
                    (unsigned char)cd.GetGreen(),
                    (unsigned char)cd.GetBlue(),
                    (unsigned char)alpha);
}

inline wxColour wxColourFromCD(ColourDesired cd)
{
    unsigned int r = cd.GetRed();
    unsigned int g = cd.GetGreen();
    unsigned int b = cd.GetBlue();
    return wxColour((unsigned char)r, (unsigned char)g, (unsigned char)b);
}

inline wxRect wxRectFromPRectangle(PRectangle prc)
{
    return wxRect(prc.left, prc.top, prc.Width(), prc.Height());
}

inline PRectangle PRectangleFromwxRect(wxRect rc)
{
    return PRectangle(rc.GetLeft(), rc.GetTop(), rc.GetRight()+1, rc.GetBottom()+1);
}

inline Point Point::FromLong(long lpoint)
{
    return Point(lpoint & 0xFFFF, lpoint >> 16);
}

#ifdef SCI_NAMESPACE
}
#endif

#endif
