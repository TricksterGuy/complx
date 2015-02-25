#include "LC3EditFrame.hpp"
#include "EditorPanel.hpp"
#include "resources.hpp"
#include <wx/filename.h>

/**
  *
  *
  */
LC3EditFrame::LC3EditFrame(const wxArrayString& files) : LC3EditFrameDecl(NULL)
{
    SetupToolbar();
    for (unsigned int i = 0; i < files.size(); i++)
        DoLoadFile(files[i]);
}

/**
  *
  *
  */
LC3EditFrame::~LC3EditFrame()
{
	// Connect Events
	Disconnect(ID_NEW, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnNew));
	Disconnect(ID_OPEN, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnOpen));
	Disconnect(ID_SAVE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnSave));
	Disconnect(ID_SAVE_ALL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnSaveAll));
	Disconnect(ID_UNDO, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnUndo));
	Disconnect(ID_REDO, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnRedo));
	Disconnect(ID_CUT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnCut));
	Disconnect(ID_COPY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnCopy));
	Disconnect(ID_PASTE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnPaste));
	Disconnect(ID_FIND, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnFind));
	Disconnect(ID_REPLACE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnReplace));
	Disconnect(ID_ASSEMBLE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnAssemble));
	Disconnect(ID_RUN, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnRun));
}

/** Loads a file.
  */
void LC3EditFrame::DoLoadFile(const wxString& file)
{
    wxFileName filename(file);
	wxPanel* panel = new EditorPanel(filename.GetFullPath(), fileNotebook);
	fileNotebook->AddPage(panel, filename.GetFullName(), false);
}


/**
  *
  *
  */
void LC3EditFrame::SetupToolbar()
{
	toolbar = CreateToolBar(wxTB_HORIZONTAL, wxID_ANY);
	toolbar->AddTool(ID_NEW, "New File", wxBitmap(new_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddTool(ID_OPEN, "Open File", wxBitmap(open_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddTool(ID_SAVE, "Save", wxBitmap(save_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddTool(ID_SAVE_ALL, "Save All", wxBitmap(saveall_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddSeparator();
	toolbar->AddTool(ID_UNDO, "Undo", wxBitmap(undo_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	toolbar->AddTool(ID_REDO, "Redo", wxBitmap(redo_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
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

	// Connect Events
	Connect(ID_NEW, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnNew));
	Connect(ID_OPEN, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnOpen));
	Connect(ID_SAVE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnSave));
	Connect(ID_SAVE_ALL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnSaveAll));
	Connect(ID_UNDO, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnUndo));
	Connect(ID_REDO, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnRedo));
	Connect(ID_CUT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnCut));
	Connect(ID_COPY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnCopy));
	Connect(ID_PASTE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnPaste));
	Connect(ID_FIND, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnFind));
	Connect(ID_REPLACE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnReplace));
	Connect(ID_ASSEMBLE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnAssemble));
	Connect(ID_RUN, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LC3EditFrame::OnRun));
}
