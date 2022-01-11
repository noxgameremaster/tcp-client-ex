
#include "pch.h"
#include "filePanel.h"
#include "downloadFileInfo.h"
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
    
    m_fileView.GetClientRect(&viewArea);

    int width = viewArea.right;
    auto setWidthFx = [&width](int columnWidth)
    {
        width = (width < columnWidth) ? 0 : width - columnWidth;
        return (width == 0) ? width : columnWidth;
    };

    cols.Append("filename", setWidthFx(200));
    cols.Append("filesize", setWidthFx(100));
    cols.Append("downloadBytes", setWidthFx(100));
    cols.Append("", width);

    m_fileView.AttachListColumn(cols);
}

void FilePanel::OnInitialUpdate()
{
    DownloadFileInfo *elem = new DownloadFileInfo;
    int columnIndex = 0;

    elem->SetElement(columnIndex++, "test_file.txt");
    elem->SetElement(columnIndex++, "300");
    elem->SetElement(columnIndex++, "0");

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


