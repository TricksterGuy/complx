////////////////////////////////////////////////////////////////////////////
// Name:        ScintillaWX.cxx
// Purpose:     A wxWidgets implementation of Scintilla.  A class derived
//              from ScintillaBase that uses the "wx platform" defined in
//              PlatformWX.cxx  This class is one end of a bridge between
//              the wx world and the Scintilla world.  It needs a peer
//              object of type wxScintilla to function.
//
// Author:      Robin Dunn
//
// Created:     13-Jan-2000
// RCS-ID:      $Id: ScintillaWX.cpp 8886 2013-02-28 05:53:24Z mortenmacfly $
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/scrolbar.h>
    #include <wx/menu.h>
    #include <wx/timer.h>
#endif // WX_PRECOMP

#include <wx/clipbrd.h>
#include <wx/textbuf.h>

#include "wx/wxscintilla.h"
#include "ScintillaWX.h"
#include "PlatWX.h"
#ifdef SCI_LEXER
#include "LexerModule.h"
#include "ExternalLexer.h"
#endif

#ifdef __WXMSW__
    // GetHwndOf()
    #include <wx/msw/private.h>
#endif

#if wxUSE_POPUPWIN
#include <wx/popupwin.h>
#define wxSCICallTipBase wxPopupWindow
#else
#include <wx/frame.h>
#define wxSCICallTipBase wxFrame
#endif

#include "wx/panel.h"
#include <wx/dcbuffer.h>

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

//----------------------------------------------------------------------
// Helper classes

class wxSCITimer : public wxTimer {
public:
    wxSCITimer(ScintillaWX* swx) {
        m_swx = swx;
    }

    void Notify() {
        m_swx->DoTick();
    }

private:
    ScintillaWX* m_swx;
};


#if wxUSE_DRAG_AND_DROP
bool wxSCIDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data) {
    return m_swx->DoDropText(x, y, data);
}

wxDragResult  wxSCIDropTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult def) {
    return m_swx->DoDragEnter(x, y, def);
}

wxDragResult  wxSCIDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def) {
    return m_swx->DoDragOver(x, y, def);
}

void  wxSCIDropTarget::OnLeave() {
    m_swx->DoDragLeave();
}
#endif // wxUSE_DRAG_AND_DROP

//--------------------------------------------------------------------------

/* C::B begin */
class wxSCICallTipContent : public wxPanel {
public:
    wxSCICallTipContent(wxWindow* parent, CallTip* ct, ScintillaWX* swx)
        : wxPanel(parent, -1),
          m_ct(ct), m_swx(swx)
    {
        SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    }

    bool AcceptsFocus() const { return false; }

    void OnPaint(wxPaintEvent& WXUNUSED(evt))
    {
        wxAutoBufferedPaintDC dc(this);
/* C::B begin */
        Surface* surfaceWindow = Surface::Allocate(SC_TECHNOLOGY_DEFAULT);
/* C::B end */
        surfaceWindow->Init(&dc, m_ct->wDraw.GetID());
        m_ct->PaintCT(surfaceWindow);
        surfaceWindow->Release();
        delete surfaceWindow;
    }

    void OnFocus(wxFocusEvent& event)
    {
        GetGrandParent()->SetFocus();
        event.Skip();
    }

    void OnLeftDown(wxMouseEvent& event)
    {
        wxPoint pt = event.GetPosition();
        Point p(pt.x, pt.y);
        m_ct->MouseClick(p);
        m_swx->CallTipClick();
    }

private:
    CallTip*     m_ct;
    ScintillaWX* m_swx;
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(wxSCICallTipContent, wxPanel)
    EVT_PAINT(wxSCICallTipContent::OnPaint)
    EVT_SET_FOCUS(wxSCICallTipContent::OnFocus)
    EVT_LEFT_DOWN(wxSCICallTipContent::OnLeftDown)
END_EVENT_TABLE()
/* C::B end */



class wxSCICallTip : public wxSCICallTipBase {
public:
    wxSCICallTip(wxWindow* parent, CallTip* ct, ScintillaWX* swx) :
#if wxUSE_POPUPWIN
        wxSCICallTipBase(parent, wxBORDER_NONE),
#else
        wxSCICallTipBase(parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                         wxFRAME_NO_TASKBAR
                         | wxFRAME_FLOAT_ON_PARENT
                         | wxBORDER_NONE
#ifdef __WXMAC__
                         | wxPOPUP_WINDOW
#endif
            ),
#endif
        m_cx(wxDefaultCoord), m_cy(wxDefaultCoord)
        {
            SetBackgroundStyle(wxBG_STYLE_CUSTOM);
            m_content = new wxSCICallTipContent(this, ct, swx);
        }

    ~wxSCICallTip() {
#if wxUSE_POPUPWIN && defined(__WXGTK__)
        wxRect rect = GetRect();
        rect.x = m_cx;
        rect.y = m_cy;
        GetParent()->Refresh(false, &rect);
#endif
    }

    bool AcceptsFocus() const { return false; }

    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO)
    {
        // convert coords to screen coords since we're a top-level window
        if (x != wxDefaultCoord) {
            m_cx = x;
            GetParent()->ClientToScreen(&x, NULL);
        }
        if (y != wxDefaultCoord) {
            m_cy = y;
            GetParent()->ClientToScreen(NULL, &y);
        }
        wxSCICallTipBase::DoSetSize(x, y, width, height, sizeFlags);
/* C::B begin */
        m_content->SetSize(0, 0, width, height, sizeFlags);
/* C::B end */
    }

#if wxUSE_POPUPWIN
#else
    virtual bool Show( bool show = true )
    {
        // Although we're a frame, we always want the parent to be active, so
        // raise it whenever we get shown.
        bool rv = wxSCICallTipBase::Show(show);
        if (rv && show)
        {
            wxTopLevelWindow *frame = wxDynamicCast(
                wxGetTopLevelParent(GetParent()), wxTopLevelWindow);
            if (frame)
                frame->Raise();
        }
        return rv;
    }
#endif

    wxPoint GetMyPosition()
    {
        return wxPoint(m_cx, m_cy);
    }

private:
    int           m_cx, m_cy;
/* C::B begin */
    wxSCICallTipContent* m_content;
/* C::B end */
};



//----------------------------------------------------------------------

#if wxUSE_DATAOBJ
static wxTextFileType wxConvertEOLMode(int scintillaMode)
{
    wxTextFileType type;

    switch (scintillaMode) {
        case wxSCI_EOL_CRLF:
            type = wxTextFileType_Dos;
            break;

        case wxSCI_EOL_CR:
            type = wxTextFileType_Mac;
            break;

        case wxSCI_EOL_LF:
            type = wxTextFileType_Unix;
            break;

        default:
            type = wxTextBuffer::typeDefault;
            break;
    }
    return type;
}
#endif // wxUSE_DATAOBJ


/* C::B begin */
static int wxCountLines(const char* text, int scintillaMode)
{
    char eolchar;

    switch (scintillaMode) {
        case wxSCI_EOL_CRLF:
        case wxSCI_EOL_LF:
            eolchar = '\n';
            break;
        case wxSCI_EOL_CR:
            eolchar = '\r';
            break;
        default:
            return 0;
    }

    int count = 0;
    int i     = 0;
    while (text[i] != 0) {
        if (text[i] == eolchar) {
            count++;
        }
        i++;
    }

    return count;
}
/* C::B end */

//----------------------------------------------------------------------
// Constructor/Destructor


ScintillaWX::ScintillaWX(wxScintilla* win) {
    capturedMouse = false;
    focusEvent = false;
    wMain = win;
    sci   = win;
    wheelRotation = 0;
    Initialise();
#ifdef __WXMSW__
    sysCaretBitmap = 0;
    sysCaretWidth = 0;
    sysCaretHeight = 0;
#endif
}


ScintillaWX::~ScintillaWX() {
    Finalise();
}

//----------------------------------------------------------------------
// base class virtuals


void ScintillaWX::Initialise() {
    //ScintillaBase::Initialise();
#if wxUSE_DRAG_AND_DROP
    dropTarget = new wxSCIDropTarget;
    dropTarget->SetScintilla(this);
    sci->SetDropTarget(dropTarget);
/* C::B begin */
    dragRectangle = false;
/* C::B end */
#endif // wxUSE_DRAG_AND_DROP
    vs.extraFontFlag = true;   // UseAntiAliasing
}


void ScintillaWX::Finalise() {
    ScintillaBase::Finalise();
    SetTicking(false);
    SetIdle(false);
    DestroySystemCaret();
}


void ScintillaWX::StartDrag() {
#if wxUSE_DRAG_AND_DROP
    wxString dragText = sci2wx (drag.s, drag.len);

    // Send an event to allow the drag text to be changed
    wxScintillaEvent evt(wxEVT_SCI_START_DRAG, sci->GetId());
    evt.SetEventObject (sci);
    evt.SetDragText(dragText);
    evt.SetDragFlags(wxDrag_DefaultMove);
    evt.SetPosition (wxMin(sci->GetSelectionStart(),
	                       sci->GetSelectionEnd()));
    sci->GetEventHandler()->ProcessEvent (evt);

/* C::B begin */
    UndoGroup ug(pdoc);
/* C::B end */

    dragText = evt.GetDragText();
/* C::B begin */
    dragRectangle = drag.rectangular;
/* C::B end */
    if ( !dragText.empty() ) {
        wxDropSource        source(sci);
        wxTextDataObject    data(dragText);
        wxDragResult        result;

        source.SetData(data);
        dropWentOutside = true;
        inDragDrop = ddDragging;
        result = source.DoDragDrop(evt.GetDragFlags());
        if (result == wxDragMove && dropWentOutside)
            ClearSelection();
        inDragDrop = ddNone;
        SetDragPosition(SelectionPosition(invalidPosition));
/* C::B begin */
        wxScintillaEvent evt(wxEVT_SCI_FINISHED_DRAG, sci->GetId());
        evt.SetEventObject (sci);
        sci->GetEventHandler()->ProcessEvent (evt);
/* C::B end */
    }
#endif // wxUSE_DRAG_AND_DROP
}


bool ScintillaWX::SetIdle(bool on) {
    if (idler.state != on) {
        // connect or disconnect the EVT_IDLE handler
        if (on)
            sci->Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(wxScintilla::OnIdle));
        else
            sci->Disconnect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(wxScintilla::OnIdle));
        idler.state = on;
    }
    return idler.state;
}


void ScintillaWX::SetTicking(bool on) {
    wxSCITimer* sciTimer;
    if (timer.ticking != on) {
        timer.ticking = on;
        if (timer.ticking) {
            sciTimer = new wxSCITimer(this);
            sciTimer->Start(timer.tickSize);
            timer.tickerID = sciTimer;
        } else {
            sciTimer = (wxSCITimer*)timer.tickerID;
            sciTimer->Stop();
            delete sciTimer;
            timer.tickerID = 0;
        }
    }
    timer.ticksToWait = caret.period;
}


void ScintillaWX::SetMouseCapture(bool on) {
    if (mouseDownCaptures) {
        if (on && !capturedMouse)
            sci->CaptureMouse();
        else if (!on && capturedMouse && sci->HasCapture())
            sci->ReleaseMouse();
        capturedMouse = on;
    }
}


bool ScintillaWX::HaveMouseCapture() {
    return capturedMouse;
}


void ScintillaWX::ScrollText(int linesToMove) {
    int dy = vs.lineHeight * (linesToMove);
    sci->ScrollWindow(0, dy);
}

void ScintillaWX::SetVerticalScrollPos() {
    if (sci->m_vScrollBar == NULL) {  // Use built-in scrollbar
        sci->SetScrollPos(wxVERTICAL, topLine);
    }
    else { // otherwise use the one that's been given to us
        sci->m_vScrollBar->SetThumbPosition(topLine);
    }
}

void ScintillaWX::SetHorizontalScrollPos() {
    if (sci->m_hScrollBar == NULL) {  // Use built-in scrollbar
        sci->SetScrollPos(wxHORIZONTAL, xOffset);
    }
    else { // otherwise use the one that's been given to us
        sci->m_hScrollBar->SetThumbPosition(xOffset);
    }
}


const int H_SCROLL_STEP = 20;

bool ScintillaWX::ModifyScrollBars(int nMax, int nPage) {
    bool modified = false;

    int vertEnd = nMax;
    if (!verticalScrollBarVisible)
        vertEnd = 0;

    // Check the vertical scrollbar
    if (sci->m_vScrollBar == NULL) {  // Use built-in scrollbar
        int  sbMax    = sci->GetScrollRange(wxVERTICAL);
        int  sbThumb  = sci->GetScrollThumb(wxVERTICAL);
        int  sbPos    = sci->GetScrollPos(wxVERTICAL);
        if (sbMax != vertEnd || sbThumb != nPage) {
            sci->SetScrollbar(wxVERTICAL, sbPos, nPage, vertEnd+1);
            modified = true;
        }
    }
    else { // otherwise use the one that's been given to us
        int  sbMax    = sci->m_vScrollBar->GetRange();
        int  sbPage   = sci->m_vScrollBar->GetPageSize();
        int  sbPos    = sci->m_vScrollBar->GetThumbPosition();
        if (sbMax != vertEnd || sbPage != nPage) {
            sci->m_vScrollBar->SetScrollbar(sbPos, nPage, vertEnd+1, nPage);
            modified = true;
        }
    }


    // Check the horizontal scrollbar
    PRectangle rcText = GetTextRectangle();
    int horizEnd = scrollWidth;
    if (horizEnd < 0)
        horizEnd = 0;
    if (!horizontalScrollBarVisible || (wrapState != eWrapNone))
        horizEnd = 0;
    int pageWidth = rcText.Width();

    if (sci->m_hScrollBar == NULL) {  // Use built-in scrollbar
        int sbMax    = sci->GetScrollRange(wxHORIZONTAL);
        int sbThumb  = sci->GetScrollThumb(wxHORIZONTAL);
        int sbPos    = sci->GetScrollPos(wxHORIZONTAL);
        if ((sbMax != horizEnd) || (sbThumb != pageWidth) || (sbPos != 0)) {
            sci->SetScrollbar(wxHORIZONTAL, sbPos, pageWidth, horizEnd);
            modified = true;
            if (scrollWidth < pageWidth) {
                HorizontalScrollTo(0);
            }
        }
    }
    else { // otherwise use the one that's been given to us
        int sbMax    = sci->m_hScrollBar->GetRange();
        int sbThumb  = sci->m_hScrollBar->GetPageSize();
        int sbPos    = sci->m_hScrollBar->GetThumbPosition();
        if ((sbMax != horizEnd) || (sbThumb != pageWidth) || (sbPos != 0)) {
            sci->m_hScrollBar->SetScrollbar(sbPos, pageWidth, horizEnd, pageWidth);
            modified = true;
            if (scrollWidth < pageWidth) {
                HorizontalScrollTo(0);
            }
        }
    }

    return modified;
}

/* C::B begin */
void ScintillaWX::NotifyFocus(bool focus) {
    sci->NotifyFocus(focus);
}
/* C::B end */

void ScintillaWX::NotifyChange() {
    sci->NotifyChange();
}


void ScintillaWX::NotifyParent(SCNotification scn) {
    sci->NotifyParent(&scn);
}


// This method is overloaded from ScintillaBase in order to prevent the
// AutoComplete window from being destroyed when it gets the focus.  There is
// a side effect that the AutoComp will also not be destroyed when switching
// to another window, but I think that is okay.
void ScintillaWX::CancelModes() {
    if (! focusEvent)
        AutoCompleteCancel();
    ct.CallTipCancel();
    Editor::CancelModes();
}



void ScintillaWX::Copy() {
/* C::B begin */
    if (!SelectionEmpty()) {
/* C::B end */
        SelectionText st;
        CopySelectionRange(&st);
/* C::B begin */
#ifdef __WXGTK__
		for (int i=0; i<5; i++)
        	CopyToClipboard(st);
#else
     	CopyToClipboard(st);
#endif
/* C::B end */
    }
}


void ScintillaWX::Paste() {
/* C::B begin */
    UndoGroup ug(pdoc);
	// Selection::First
    SelectionPosition firstPosition = SelectionStart();
    if (sel.IsRectangular() && !sel.Empty()) {
        for (size_t i=0; i<sel.Count()-1; i++) {
            sel.RotateMain();
            if (firstPosition > SelectionStart())
                firstPosition = SelectionStart();
        }
    }
/* C::B end */
    ClearSelection();

#if wxUSE_DATAOBJ
    wxTextDataObject data;
/* C::B begin */
    wxString textString;

    bool  rectangular = false;

    wxTheClipboard->UsePrimarySelection(false);
    if (wxTheClipboard->Open()) {
        // Leave the following lines that way to enable compilation with GCC 3.3.3
        wxDataFormat dataFormat(wxString(wxT("application/x-cbrectdata")));
        wxCustomDataObject selData(dataFormat);
        bool gotRectData = wxTheClipboard->GetData(selData);

        if (gotRectData && selData.GetSize()>1) {
            wxTheClipboard->Close();
            const char* rectBuf = (const char*)selData.GetData();
            rectangular = rectBuf[0] == (char)1;
            int len = selData.GetDataSize()-1;
            char* buffer = new char[len];
            memcpy (buffer, rectBuf+1, len);
            textString = sci2wx(buffer, len);
            delete [] buffer;
        } else {
            bool gotData = wxTheClipboard->GetData(data);
            wxTheClipboard->Close();
            if (gotData) {
                textString = wxTextBuffer::Translate (data.GetText(),
                                                      wxConvertEOLMode(pdoc->eolMode));
            }
        }
    }

    wxWX2MBbuf buf = (wxWX2MBbuf)wx2sci(textString);
#if wxUSE_UNICODE
    // free up the old character buffer in case the text is real big
    data.SetText(wxEmptyString);
    textString = wxEmptyString;
#endif
    int len  = strlen(buf);
    int newPos = 0;
    int caretMain = CurrentPosition();
    if (rectangular) {
        SelectionPosition selStart = sel.Range(sel.Main()).Start();
        int newLine = pdoc->LineFromPosition (caretMain) + wxCountLines (buf, pdoc->eolMode);
        int newCol = pdoc->GetColumn(caretMain);
        PasteRectangular (selStart, buf, len);
        newPos = pdoc->FindColumn (newLine, newCol);
    } else {
        pdoc->InsertString (caretMain, buf, len);
        newPos = caretMain + len;
    }
    SetEmptySelection (newPos);
/* C::B end */
#endif // wxUSE_DATAOBJ

    NotifyChange();
    Redraw();
}


void ScintillaWX::CopyToClipboard(const SelectionText& st) {
#if wxUSE_CLIPBOARD
    if ( !st.len )
        return;

    wxTheClipboard->UsePrimarySelection(false);
    if (wxTheClipboard->Open()) {
        wxString text = wxTextBuffer::Translate(sci2wx(st.s, st.len-1));

/* C::B begin */
        // composite object will hold "plain text" for pasting in other programs and a custom
        // object for local use that remembers what kind of selection was made (stream or
        // rectangular).
        wxDataObjectComposite* obj = new wxDataObjectComposite();
        wxCustomDataObject* rectData = new wxCustomDataObject (wxDataFormat(wxString(wxT("application/x-cbrectdata"))));

        char* buffer = new char[st.len+1];
        buffer[0] = (st.rectangular)? (char)1 : (char)0;
        memcpy (buffer+1, st.s, st.len);
        rectData->SetData (st.len+1, buffer);
        delete [] buffer;

        obj->Add (rectData, true);
        obj->Add (new wxTextDataObject (text));
        wxTheClipboard->SetData (obj);
/* C::B end */
        wxTheClipboard->Close();
    }
#else
    wxUnusedVar(st);
#endif // wxUSE_CLIPBOARD
}


bool ScintillaWX::CanPaste() {
#if wxUSE_CLIPBOARD
    bool canPaste = false;
    bool didOpen;

    if (Editor::CanPaste()) {
        wxTheClipboard->UsePrimarySelection(false);
        didOpen = !wxTheClipboard->IsOpened();
        if ( didOpen )
            wxTheClipboard->Open();

        if (wxTheClipboard->IsOpened()) {
            canPaste = wxTheClipboard->IsSupported(wxUSE_UNICODE ? wxDF_UNICODETEXT : wxDF_TEXT);
            if (didOpen)
                wxTheClipboard->Close();
        }
    }
    return canPaste;
#else
    return false;
#endif // wxUSE_CLIPBOARD
}

void ScintillaWX::CreateCallTipWindow(PRectangle) {
    if (! ct.wCallTip.Created() ) {
        ct.wCallTip = new wxSCICallTip(sci, &ct, this);
        ct.wDraw = ct.wCallTip;
    }
}


void ScintillaWX::AddToPopUp(const char *label, int cmd, bool enabled) {
    if (!label[0])
        ((wxMenu*)popup.GetID())->AppendSeparator();
    else
        ((wxMenu*)popup.GetID())->Append(cmd, wxGetTranslation(sci2wx(label)));

    if (!enabled)
        ((wxMenu*)popup.GetID())->Enable(cmd, enabled);
}


// This is called by the Editor base class whenever something is selected.
// For wxGTK we can put this text in the primary selection and then other apps
// can paste with the middle button.
void ScintillaWX::ClaimSelection() {
#if 0
#ifdef __WXGTK__
    // Put the selected text in the PRIMARY selection
/* C::B begin */
    if (!SelectionEmpty()) {
/* C::B end */
        SelectionText st;
        CopySelectionRange(&st);
        wxTheClipboard->UsePrimarySelection(true);
        if (wxTheClipboard->Open()) {
            wxString text = sci2wx(st.s, st.len);
            wxTheClipboard->SetData(new wxTextDataObject(text));
            wxTheClipboard->Close();
        }
        wxTheClipboard->UsePrimarySelection(false);
    }
#endif
#endif
}


void ScintillaWX::UpdateSystemCaret() {
#ifdef __WXMSW__
    if (hasFocus) {
        if (HasCaretSizeChanged()) {
            DestroySystemCaret();
            CreateSystemCaret();
        }
        Point pos = PointMainCaret();
        ::SetCaretPos(pos.x, pos.y);
    }
#endif
}


bool ScintillaWX::HasCaretSizeChanged() {
#ifdef __WXMSW__
/* C::B begin */
    if (    ( (0 != vs.caretWidth) && (sysCaretWidth  != vs.caretWidth) )
         || ( (0 != vs.lineHeight) && (sysCaretHeight != vs.lineHeight) ) ) {
/* C::B end */
        return true;
    }
#endif
    return false;
}

bool ScintillaWX::CreateSystemCaret() {
#ifdef __WXMSW__
    sysCaretWidth = vs.caretWidth;
    if (0 == sysCaretWidth) {
        sysCaretWidth = 1;
    }
    sysCaretHeight = vs.lineHeight;
    int bitmapSize = (((sysCaretWidth + 15) & ~15) >> 3) * sysCaretHeight;
    char *bits = new char[bitmapSize];
    memset(bits, 0, bitmapSize);
    sysCaretBitmap = ::CreateBitmap(sysCaretWidth, sysCaretHeight, 1,
                                    1, reinterpret_cast<BYTE *>(bits));
    delete [] bits;
    BOOL retval = ::CreateCaret(GetHwndOf(sci), sysCaretBitmap,
                                sysCaretWidth, sysCaretHeight);
    ::ShowCaret(GetHwndOf(sci));
    return retval != 0;
#else
    return false;
#endif
}

bool ScintillaWX::DestroySystemCaret() {
#ifdef __WXMSW__
    ::HideCaret(GetHwndOf(sci));
    BOOL retval = ::DestroyCaret();
    if (sysCaretBitmap) {
        ::DeleteObject(sysCaretBitmap);
        sysCaretBitmap = 0;
    }
    return retval != 0;
#else
    return false;
#endif
}


//----------------------------------------------------------------------


sptr_t ScintillaWX::DefWndProc(unsigned int /*iMessage*/, uptr_t /*wParam*/, sptr_t /*lParam*/) {
    return 0;
}

sptr_t ScintillaWX::WndProc(unsigned int iMessage, uptr_t wParam, sptr_t lParam) {
      switch (iMessage) {
      case SCI_CALLTIPSHOW: {
          // NOTE: This is copied here from scintilla/src/ScintillaBase.cxx
          // because of the little tweak that needs done below for wxGTK.
          // When updating new versions double check that this is still
          // needed, and that any new code there is copied here too.
          Point pt = LocationFromPosition(wParam);
          char* defn = reinterpret_cast<char *>(lParam);
          AutoCompleteCancel();
/* C::B begin */
          //pt.y += vs.lineHeight;
/* C::B end */
/* C::B begin */
          /* This fix will allow you to actually modify the calltip font (it
           * was hardcoded to STYLE_DEFAULT instead of allowing user to
           * override it when STYLE_CALLTIP is set). */
          int ctStyle = ct.UseStyleCallTip() ? STYLE_CALLTIP : STYLE_DEFAULT;
          if (ct.UseStyleCallTip())
          {
              ct.SetForeBack(vs.styles[STYLE_CALLTIP].fore, vs.styles[STYLE_CALLTIP].back);
          }
/* C::B end */
          int caretMain = sel.MainCaret();
          PRectangle rc = ct.CallTipStart(caretMain, pt,
                                          vs.lineHeight,
                                          defn,
                                          vs.styles[ctStyle].fontName,
                                          vs.styles[ctStyle].sizeZoomed,
                                          CodePage(),
                                          vs.styles[ctStyle].characterSet,
                                          vs.technology,
                                          wMain);
          // If the call-tip window would be out of the client
          // space, adjust so it displays above the text.
          PRectangle rcClient = GetClientRectangle();
          if (rc.bottom > rcClient.bottom) {
#ifdef __WXGTK__
              int offset = int(vs.lineHeight * 1.25)  + rc.Height();
#else
              int offset = vs.lineHeight + rc.Height();
#endif
              rc.top -= offset;
              rc.bottom -= offset;
          }
/* C::B begin */
          // If the call-tip window would be out of the client space to
          // the right, adjust it so right egde is at the right edge of
          // the client.
          if (rc.right > rcClient.right) {
              rc.left -= rc.right - rcClient.right;
              rc.right = rcClient.right;
          }
/* C::B end */
          // Now display the window.
          CreateCallTipWindow(rc);
          ct.wCallTip.SetPositionRelative(rc, wMain);
          ct.wCallTip.Show();
          break;
      }

/* C::B begin */
      case SCI_GETDIRECTFUNCTION:
            return reinterpret_cast<sptr_t>(DirectFunction);

      case SCI_GETDIRECTPOINTER:
            return reinterpret_cast<sptr_t>(this);

      case SCI_GRABFOCUS:
            sci->SetFocus();
            break;
/* C::B end */

#ifdef SCI_LEXER
      case SCI_LOADLEXERLIBRARY:
            LexerManager::GetInstance()->Load((const char*)lParam);
            break;
#endif

      default:
          return ScintillaBase::WndProc(iMessage, wParam, lParam);
      }
      return 0;
}

/* C::B begin */
sptr_t ScintillaWX::DirectFunction(ScintillaWX *wxsci, unsigned int iMessage, uptr_t wParam, sptr_t lParam)
{
	return wxsci->WndProc(iMessage, wParam, lParam);
}
/* C::B end */


//----------------------------------------------------------------------
// Event delegates

void ScintillaWX::DoPaint(wxDC* dc, wxRect rect) {

    paintState = painting;
    AutoSurface surfaceWindow(dc, this);
    if (surfaceWindow) {
        rcPaint = PRectangleFromwxRect(rect);
        PRectangle rcClient = GetClientRectangle();
        paintingAllText = rcPaint.Contains(rcClient);

        ClipChildren(*dc, rcPaint);
        Paint(surfaceWindow, rcPaint);
        surfaceWindow->Release();
    }

    if (paintState == paintAbandoned) {
        // Painting area was insufficient to cover new styling or brace
        // highlight positions.  So trigger a new paint event that will
        // repaint the whole window.
        sci->Refresh(false);

    /* C::B begin */
    // This used to be NOT within the #if defined(__WXOSX__):
#if defined(__WXOSX__)
        // On Mac we also need to finish the current paint to make sure that
        // everything is on the screen that needs to be there between now and
        // when the next paint event arrives.
        FullPaintDC(dc);
#endif
    /* C::B end */
    }
    paintState = notPainting;
}


// Force the whole window to be repainted
void ScintillaWX::FullPaint() {
    /* C::B begin */
    // This used to be:
//    wxClientDC dc(sci);
//    FullPaintDC(&dc);
    /* C::B end */
    sci->Refresh(false);
    sci->Update();
}


void ScintillaWX::FullPaintDC(wxDC* dc) {
    paintState = painting;
    rcPaint = GetClientRectangle();
    paintingAllText = true;
    AutoSurface surfaceWindow(dc, this);
    if (surfaceWindow) {
        Paint(surfaceWindow, rcPaint);
        surfaceWindow->Release();
    }
    paintState = notPainting;
}



void ScintillaWX::DoHScroll(int type, int pos) {
    int xPos = xOffset;
    PRectangle rcText = GetTextRectangle();
    int pageWidth = rcText.Width() * 2 / 3;
    if (type == wxEVT_SCROLLWIN_LINEUP || type == wxEVT_SCROLL_LINEUP)
        xPos -= H_SCROLL_STEP;
    else if (type == wxEVT_SCROLLWIN_LINEDOWN || type == wxEVT_SCROLL_LINEDOWN)
        xPos += H_SCROLL_STEP;
    else if (type == wxEVT_SCROLLWIN_PAGEUP || type == wxEVT_SCROLL_PAGEUP)
        xPos -= pageWidth;
    else if (type == wxEVT_SCROLLWIN_PAGEDOWN || type == wxEVT_SCROLL_PAGEDOWN) {
        xPos += pageWidth;
        if (xPos > scrollWidth - rcText.Width()) {
            xPos = scrollWidth - rcText.Width();
        }
    }
    else if (type == wxEVT_SCROLLWIN_TOP || type == wxEVT_SCROLL_TOP)
        xPos = 0;
    else if (type == wxEVT_SCROLLWIN_BOTTOM || type == wxEVT_SCROLL_BOTTOM)
        xPos = scrollWidth;
    else if (type == wxEVT_SCROLLWIN_THUMBTRACK || type == wxEVT_SCROLL_THUMBTRACK)
        xPos = pos;

    HorizontalScrollTo(xPos);
}

void ScintillaWX::DoVScroll(int type, int pos) {
    int topLineNew = topLine;
    if (type == wxEVT_SCROLLWIN_LINEUP || type == wxEVT_SCROLL_LINEUP)
        topLineNew -= 1;
    else if (type == wxEVT_SCROLLWIN_LINEDOWN || type == wxEVT_SCROLL_LINEDOWN)
        topLineNew += 1;
    else if (type ==  wxEVT_SCROLLWIN_PAGEUP || type == wxEVT_SCROLL_PAGEUP)
        topLineNew -= LinesToScroll();
    else if (type ==  wxEVT_SCROLLWIN_PAGEDOWN || type == wxEVT_SCROLL_PAGEDOWN)
        topLineNew += LinesToScroll();
    else if (type ==  wxEVT_SCROLLWIN_TOP || type == wxEVT_SCROLL_TOP)
        topLineNew = 0;
    else if (type ==  wxEVT_SCROLLWIN_BOTTOM || type == wxEVT_SCROLL_BOTTOM)
        topLineNew = MaxScrollPos();
    else if (type ==   wxEVT_SCROLLWIN_THUMBTRACK || type == wxEVT_SCROLL_THUMBTRACK)
        topLineNew = pos;

    ScrollTo(topLineNew);
}

void ScintillaWX::DoMouseWheel(int rotation, int delta,
                               int linesPerAction, int ctrlDown,
                               bool isPageScroll ) {
    int topLineNew = topLine;
    int lines;

    if (ctrlDown) {  // Zoom the fonts if Ctrl key down
/* C::B begin */
        if (rotation < 0) {
/* C::B end */
            KeyCommand(SCI_ZOOMIN);
        }
        else {
            KeyCommand(SCI_ZOOMOUT);
        }
    }
    else { // otherwise just scroll the window
        if ( !delta )
            delta = 120;
        wheelRotation += rotation;
        lines = wheelRotation / delta;
        wheelRotation -= lines * delta;
        if (lines != 0) {
            if (isPageScroll)
                lines = lines * LinesOnScreen();  // lines is either +1 or -1
            else
                lines *= linesPerAction;
            topLineNew -= lines;
            ScrollTo(topLineNew);
        }
    }
}


void ScintillaWX::DoSize(int WXUNUSED(width), int WXUNUSED(height)) {
    ChangeSize();
}

void ScintillaWX::DoLoseFocus(){
    focusEvent = true;
    SetFocusState(false);
    focusEvent = false;
/* C::B begin */
    SetMouseCapture(false);
/* C::B end */
    DestroySystemCaret();
}

void ScintillaWX::DoGainFocus(){
    focusEvent = true;
    SetFocusState(true);
    focusEvent = false;
    DestroySystemCaret();
    CreateSystemCaret();
}

void ScintillaWX::DoSysColourChange() {
    InvalidateStyleData();
}

void ScintillaWX::DoLeftButtonDown(Point pt, unsigned int curTime, bool shift, bool ctrl, bool alt) {
    ButtonDown(pt, curTime, shift, ctrl, alt);
}

void ScintillaWX::DoLeftButtonUp(Point pt, unsigned int curTime, bool ctrl) {
    ButtonUp(pt, curTime, ctrl);
}

void ScintillaWX::DoLeftButtonMove(Point pt) {
    ButtonMove(pt);
}

#ifdef __WXGTK__
void ScintillaWX::DoMiddleButtonUp(Point pt) {
    // Set the current position to the mouse click point and
    // then paste in the PRIMARY selection, if any.  wxGTK only.
    int newPos = PositionFromLocation(pt);
    MovePositionTo(newPos, Selection::noSel, true);

/* C::B begin */
    UndoGroup ug(pdoc);
/* C::B end */
    wxTextDataObject data;
    bool gotData = false;
    wxTheClipboard->UsePrimarySelection(true);
    if (wxTheClipboard->Open()) {
        gotData = wxTheClipboard->GetData(data);
        wxTheClipboard->Close();
    }
    wxTheClipboard->UsePrimarySelection(false);
    if (gotData) {
        wxString  text = wxTextBuffer::Translate(data.GetText(),
                                                 wxConvertEOLMode(pdoc->eolMode));
        wxWX2MBbuf buf = (wxWX2MBbuf)wx2sci(text);
        int        len = strlen(buf);
        int caretMain = sel.MainCaret();
        pdoc->InsertString(caretMain, buf, len);
        SetEmptySelection(caretMain + len);
    }
    NotifyChange();
    Redraw();

    ShowCaretAtCurrentPosition();
    EnsureCaretVisible();
}
#else
void ScintillaWX::DoMiddleButtonUp(Point WXUNUSED(pt)) {
}
#endif


void ScintillaWX::DoAddChar(int key) {
#if wxUSE_UNICODE
    wxChar wszChars[2];
    wszChars[0] = (wxChar)key;
    wszChars[1] = 0;
    wxWX2MBbuf buf = (wxWX2MBbuf)wx2sci(wszChars);
    AddCharUTF((char*)buf.data(), strlen(buf));
#else
    AddChar((char)key);
#endif
}


int  ScintillaWX::DoKeyDown(const wxKeyEvent& evt, bool* consumed)
{
    int key = evt.GetKeyCode();
/* C::B begin */
#if wxCHECK_VERSION(2, 9, 0)
    if (key == WXK_NONE) {
        // This is a Unicode character not representable in Latin-1 or some key
        // without key code at all (e.g. dead key or VK_PROCESSKEY under MSW).
        if ( consumed )
            *consumed = false;
        return 0;
    }
#endif
/* C::B end */
    bool shift = evt.ShiftDown(),
         ctrl  = evt.ControlDown(),
         alt   = evt.AltDown();

    if (ctrl && key >= 1 && key <= 26 && key != WXK_BACK)
        key += 'A' - 1;

    switch (key) {
    case WXK_DOWN:              // fall through
    case WXK_NUMPAD_DOWN:       key = SCK_DOWN;     break;
    case WXK_UP:                // fall through
    case WXK_NUMPAD_UP:         key = SCK_UP;       break;
    case WXK_LEFT:              // fall through
    case WXK_NUMPAD_LEFT:       key = SCK_LEFT;     break;
    case WXK_RIGHT:             // fall through
    case WXK_NUMPAD_RIGHT:      key = SCK_RIGHT;    break;
    case WXK_HOME:              // fall through
    case WXK_NUMPAD_HOME:       key = SCK_HOME;     break;
    case WXK_END:               // fall through
    case WXK_NUMPAD_END:        key = SCK_END;      break;
#if !wxCHECK_VERSION(2, 8, 0)
    case WXK_PRIOR:             // fall through
    case WXK_NUMPAD_PRIOR:      // fall through
#endif
    case WXK_PAGEUP:            // fall through
    case WXK_NUMPAD_PAGEUP:     key = SCK_PRIOR;    break;
#if !wxCHECK_VERSION(2, 8, 0)
    case WXK_NEXT:              // fall through
    case WXK_NUMPAD_NEXT:       // fall through
#endif
    case WXK_PAGEDOWN:          // fall through
    case WXK_NUMPAD_PAGEDOWN:   key = SCK_NEXT;     break;
    case WXK_NUMPAD_DELETE:     // fall through
    case WXK_DELETE:            key = SCK_DELETE;   break;
    case WXK_NUMPAD_INSERT:     // fall through
    case WXK_INSERT:            key = SCK_INSERT;   break;
    case WXK_ESCAPE:            key = SCK_ESCAPE;   break;
    case WXK_BACK:              key = SCK_BACK;     break;
    case WXK_TAB:               key = SCK_TAB;      break;
    case WXK_RETURN:            // fall through
    case WXK_NUMPAD_ENTER:      key = SCK_RETURN;   break;
    case WXK_ADD:               // fall through
    case WXK_NUMPAD_ADD:        key = SCK_ADD;      break;
    case WXK_SUBTRACT:          // fall through
    case WXK_NUMPAD_SUBTRACT:   key = SCK_SUBTRACT; break;
    case WXK_DIVIDE:            // fall through
    case WXK_NUMPAD_DIVIDE:     key = SCK_DIVIDE;   break;
    case WXK_CONTROL:           key = 0; break;
    case WXK_ALT:               key = 0; break;
    case WXK_SHIFT:             key = 0; break;
    case WXK_MENU:              key = SCK_MENU;     break;
    }

#ifdef __WXMAC__
    if ( evt.MetaDown() ) {
        // check for a few common Mac Meta-key combos and remap them to Ctrl
        // for Scintilla
        switch ( key ) {
        case 'Z':       // Undo
        case 'X':       // Cut
        case 'C':       // Copy
        case 'V':       // Paste
        case 'A':       // Select All
            ctrl = true;
            break;
        }
    }
#endif

    int rv = KeyDown(key, shift, ctrl, alt, consumed);

    if (key)
        return rv;
    else
        return 1;
}


void ScintillaWX::DoCommand(int ID) {
    Command(ID);
}


void ScintillaWX::DoContextMenu(Point pt) {
    if (displayPopupMenu)
        ContextMenu(pt);
}

void ScintillaWX::DoOnListBox() {
    AutoCompleteCompleted();
}


void ScintillaWX::DoOnIdle(wxIdleEvent& evt) {

    if ( Idle() )
        evt.RequestMore();
    else
        SetIdle(false);
}

//----------------------------------------------------------------------

#if wxUSE_DRAG_AND_DROP
bool ScintillaWX::DoDropText(long x, long y, const wxString& data) {
    SetDragPosition(SelectionPosition(invalidPosition));

    wxString text = wxTextBuffer::Translate(data,
                                            wxConvertEOLMode(pdoc->eolMode));

    // Send an event to allow the drag details to be changed
    wxScintillaEvent evt(wxEVT_SCI_DO_DROP, sci->GetId());
    evt.SetEventObject(sci);
    evt.SetDragResult(dragResult);
    evt.SetX(x);
    evt.SetY(y);
    evt.SetPosition(PositionFromLocation(Point(x,y)));
    evt.SetDragText(text);
    sci->GetEventHandler()->ProcessEvent(evt);

    dragResult = evt.GetDragResult();
    if (dragResult == wxDragMove || dragResult == wxDragCopy) {
        DropAt(SelectionPosition(evt.GetPosition()),
               wx2sci(evt.GetDragText()),
               dragResult == wxDragMove,
/* C::B begin */
               dragRectangle);
/* C::B end */
        return true;
    }
    return false;
}


wxDragResult ScintillaWX::DoDragEnter(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxDragResult def) {
    dragResult = def;
    return dragResult;
}


wxDragResult ScintillaWX::DoDragOver(wxCoord x, wxCoord y, wxDragResult def) {
    SetDragPosition(SelectionPosition(PositionFromLocation(Point(x, y))));

    // Send an event to allow the drag result to be changed
    wxScintillaEvent evt(wxEVT_SCI_DRAG_OVER, sci->GetId());
    evt.SetEventObject(sci);
    evt.SetDragResult(def);
    evt.SetX(x);
    evt.SetY(y);
    evt.SetPosition(PositionFromLocation(Point(x,y)));
    sci->GetEventHandler()->ProcessEvent(evt);

    dragResult = evt.GetDragResult();
    return dragResult;
}


void ScintillaWX::DoDragLeave() {
    SetDragPosition(SelectionPosition(invalidPosition));
}
#endif // wxUSE_DRAG_AND_DROP
//----------------------------------------------------------------------

void ScintillaWX::DoScrollToLine(int line) {
    ScrollTo(line);
}


void ScintillaWX::DoScrollToColumn(int column) {
    HorizontalScrollTo(column * vs.spaceWidth);
}

// wxGTK doesn't appear to need this explicit clipping code any longer, but I
// will leave it here commented out for a while just in case...
void ScintillaWX::ClipChildren(wxDC& WXUNUSED(dc), PRectangle WXUNUSED(rect))
{
//     wxRegion rgn(wxRectFromPRectangle(rect));
//     if (ac.Active()) {
//         wxRect childRect = ((wxWindow*)ac.lb->GetID())->GetRect();
//         rgn.Subtract(childRect);
//     }
//     if (ct.inCallTipMode) {
//         wxSCICallTip* tip = (wxSCICallTip*)ct.wCallTip.GetID();
//         wxRect childRect = tip->GetRect();
// #if wxUSE_POPUPWIN
//         childRect.SetPosition(tip->GetMyPosition());
// #endif
//         rgn.Subtract(childRect);
//     }
//     dc.SetClippingRegion(rgn);
}


void ScintillaWX::SetUseAntiAliasing(bool useAA) {
    vs.extraFontFlag = useAA;
    InvalidateStyleRedraw();
}

bool ScintillaWX::GetUseAntiAliasing() {
    return vs.extraFontFlag != 0;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
