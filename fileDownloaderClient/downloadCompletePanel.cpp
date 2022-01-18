
#include "pch.h"
#include "downloadCompletePanel.h"
#include "resource.h"

DownloadCompletePanel::DownloadCompletePanel(UINT nIDTemplate, CWnd *parent)
    : CTabPage(nIDTemplate, parent)
{ }

DownloadCompletePanel::~DownloadCompletePanel()
{ }

void DownloadCompletePanel::InitCControls()
{
    m_textTitle.SetWindowTextA("Downloaded Files");
    m_textTitle.SetTextColor(RGB(242, 251, 132));
}

void DownloadCompletePanel::OnInitialUpdate()
{
    ListViewer::ListColumn listHeader;
    CRect viewArea;

    m_completeList.GetClientRect(&viewArea);

    int width = viewArea.right;
    auto setWidthFx = [&width](int columnWidth)
    {
        width = (width < columnWidth) ? 0 : width - columnWidth;
        return (width == 0) ? width : columnWidth;
    };

    listHeader.Append("FileName", setWidthFx(300));
    listHeader.Append("FileSize", setWidthFx(100));
    listHeader.Append("CreatedDateTime", setWidthFx(150));
    listHeader.Append("", width);

    m_completeList.AttachListColumn(listHeader);
}

void DownloadCompletePanel::OnEnterScreen()
{ }

void DownloadCompletePanel::DoDataExchange(CDataExchange *pDX)
{
    CTabPage::DoDataExchange(pDX);
    auto controlExchange = [pDX](int nIDC, CWnd &wnd)
    { DDX_Control(pDX, nIDC, wnd); };

    controlExchange(COMPLETE_TITLE, m_textTitle);
    controlExchange(COMPLETE_LIST, m_completeList);
    InitCControls();
}