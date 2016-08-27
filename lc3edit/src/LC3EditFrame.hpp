#ifndef LC3EDITFRAME_HPP
#define LC3EDITFRAME_HPP

#include <wx/toolbar.h>
#include "LC3EditFrameDecl.h"

#define ID_NEW 1000
#define ID_OPEN 1001
#define ID_SAVE 1002
#define ID_SAVE_ALL 1003
#define ID_UNDO 1004
#define ID_REDO 1005
#define ID_CUT 1006
#define ID_COPY 1007
#define ID_PASTE 1008
#define ID_FIND 1009
#define ID_REPLACE 1010
#define ID_ASSEMBLE 1011
#define ID_RUN 1012

class LC3EditFrame : public LC3EditFrameDecl
{
    public:
        LC3EditFrame(const wxArrayString& files);
        ~LC3EditFrame();
        void DoLoadFile(const wxString& file);
    private:
        wxToolBar* toolbar;
        void SetupToolbar();
		void OnNew(wxCommandEvent& event) {event.Skip();}
		void OnOpen(wxCommandEvent& event) {event.Skip();}
		void OnSave(wxCommandEvent& event) {event.Skip();}
		void OnSaveAll(wxCommandEvent& event) {event.Skip();}
		void OnUndo(wxCommandEvent& event) {event.Skip();}
		void OnRedo(wxCommandEvent& event) {event.Skip();}
		void OnCut(wxCommandEvent& event) {event.Skip();}
		void OnCopy(wxCommandEvent& event) {event.Skip();}
		void OnPaste(wxCommandEvent& event) {event.Skip();}
		void OnFind(wxCommandEvent& event) {event.Skip();}
		void OnReplace(wxCommandEvent& event) {event.Skip();}
		void OnAssemble(wxCommandEvent& event) {event.Skip();}
		void OnRun(wxCommandEvent& event) {event.Skip();}
};

#endif
