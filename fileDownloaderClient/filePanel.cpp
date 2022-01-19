
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

    cols.Append("filename", setWidthFx(140));
    cols.Append("savepath", setWidthFx(160));
    cols.Append("filesize", setWidthFx(100));
    cols.Append("downloadBytes", setWidthFx(100));
    cols.Append("", width);

    m_fileView.AttachListColumn(cols);

    m_textTitle.SetWindowTextA("File Downloading");
    m_textTitle.SetTextColor(RGB(128, 255, 255));
}

void FilePanel::OnInitialUpdate()
{ }

void FilePanel::SlotFileListAppend(std::shared_ptr<DownloadFileInfo> &&updateItem)
{
    std::string keyString = updateItem->GetElement(DownloadFileInfo::PropertyInfo::FileName);       //Å°°ª
    std::unique_ptr<DownloadFileInfo> copiedItem(new DownloadFileInfo(*updateItem));

    m_fileView.Append(std::move(copiedItem));
}

void FilePanel::SlotFileListErase(std::shared_ptr<DownloadFileInfo> &&delItem)
{
    m_fileView.Erase(delItem);
}

BEGIN_MESSAGE_MAP(FilePanel, CTabPage)
END_MESSAGE_MAP()

void FilePanel::DoDataExchange(CDataExchange *pDX)
{
    CTabPage::DoDataExchange(pDX);

    DDX_Control(pDX, FILE_LIST_PANEL_LIST, m_fileView);
    DDX_Control(pDX, FILE_LIST_PANEL_TITLE, m_textTitle);

    InitCControls();
}


