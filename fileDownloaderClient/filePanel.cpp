
#include "pch.h"
#include "filePanel.h"
#include "listElement.h"
#include "resource.h"

FilePanel::FilePanel(UINT nIDTemplate, CWnd *parent)
    : CTabPage(nIDTemplate, parent)
{ }

FilePanel::~FilePanel()
{ }

void FilePanel::InitCControls()
{
    ListViewer::ListColumn cols;
    CRect viewArea;
    
    //GetWindowRect(viewArea);
    GetClientRect(viewArea);

    cols.Append("order", 50);
    cols.Append("filename", viewArea.Width() - 50);

    m_fileView.AttachListColumn(cols);
}

void FilePanel::OnInitialUpdate()
{
    ListElement *elem = new ListElement;

    elem->SetElement(0, "1");
    elem->SetElement(1, "test_file.txt");

    m_fileView.Append(std::unique_ptr<ListElement>(elem));
}

BEGIN_MESSAGE_MAP(FilePanel, CTabPage)
END_MESSAGE_MAP()

void FilePanel::DoDataExchange(CDataExchange *pDX)
{
    CTabPage::DoDataExchange(pDX);

    DDX_Control(pDX, FILE_LIST_PANEL_LIST, m_fileView);

    InitCControls();
}


