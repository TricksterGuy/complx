#ifndef ASM_FILE_DOCUMENT_HPP
#define ASM_FILE_DOCUMENT_HPP

#include <wx/docview.h>
#include <wx/filename.h>

class AsmFileDocument : public wxDocument
{
	DECLARE_DYNAMIC_CLASS(AsmFileDocument)
    public:
        AsmFileDocument() {}
        ~AsmFileDocument() {}
        bool DeleteContents();
    protected:
        bool DoSaveDocument(const wxString& file);
        bool DoOpenDocument(const wxString& file);
    private:
        wxFileName filename;
};

#endif

