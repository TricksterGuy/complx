#include "LC3EditFrame.hpp"
#include "EditorView.hpp"
#include "resources.hpp"
#include <wx/filename.h>
#include <wx/debug.h>

LC3EditFrame::LC3EditFrame(wxDocManager* m, const wxArrayString& files) : LC3EditFrameDecl(m, NULL), manager(m)
{
    SetupToolbar();

    for (const auto& file : files)
        DoLoadFile(file);
}

LC3EditFrame::~LC3EditFrame()
{
	Disconnect(ID_SAVE_ALL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnSaveAll));
	Disconnect(ID_CUT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnCut));
	Disconnect(ID_COPY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnCopy));
	Disconnect(ID_PASTE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnPaste));
	Disconnect(ID_FIND, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnFind));
	Disconnect(ID_REPLACE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnReplace));
	Disconnect(ID_ASSEMBLE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnAssemble));
	Disconnect(ID_RUN, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnRun));
}

void LC3EditFrame::DoLoadFile(const wxString& file)
{
    wxFileName filename(file);
    printf("%s\n", file.c_str().AsChar());
    wxDocument* doc = manager->CreateDocument(filename.GetFullPath());
    EditorView* view = new EditorView();
    doc->AddView(view);
    view->OnInit(fileNotebook);
	wxPanel* panel = new EditorPanel(fileNotebook);
	fileNotebook->AddPage(panel, filename.GetFullName(), false);
}

void LC3EditFrame::SetupToolbar()
{
	toolbar = CreateToolBar(wxTB_HORIZONTAL, wxID_ANY);
	toolbar->AddTool(wxID_NEW, "New File", wxBitmap(new_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddTool(wxID_OPEN, "Open File", wxBitmap(open_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddTool(wxID_SAVE, "Save", wxBitmap(save_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddTool(ID_SAVE_ALL, "Save All", wxBitmap(saveall_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddSeparator();
	toolbar->AddTool(wxID_UNDO, "Undo", wxBitmap(undo_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddTool(wxID_REDO, "Redo", wxBitmap(redo_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddSeparator();
	toolbar->AddTool(ID_CUT, "Cut", wxBitmap(cut_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddTool(ID_COPY, "Copy", wxBitmap(copy_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddTool(ID_PASTE, "Paste", wxBitmap(paste_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddTool(ID_FIND, "Find", wxBitmap(find_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddTool(ID_REPLACE, "Replace", wxBitmap(replace_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddSeparator();
	toolbar->AddTool(ID_ASSEMBLE, "Assemble", wxBitmap(assemble_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddTool(ID_RUN, "Run", wxBitmap(run_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->Realize();

	Connect(ID_SAVE_ALL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnSaveAll));
	Connect(ID_CUT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnCut));
	Connect(ID_COPY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnCopy));
	Connect(ID_PASTE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnPaste));
	Connect(ID_FIND, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnFind));
	Connect(ID_REPLACE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnReplace));
	Connect(ID_ASSEMBLE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnAssemble));
	Connect(ID_RUN, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnRun));
}
