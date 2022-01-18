
#include "pch.h"
#include "switchPanel.h"
#include "resource.h"

SwitchPanel::SwitchPanel(UINT nIDTemplate, CWnd *parent)
    : CTabPage(nIDTemplate, parent)
{ }

SwitchPanel::~SwitchPanel()
{ }

void SwitchPanel::OnInitialUpdate()
{
    m_downloadPageBtn.SetCallback([this]() { this->PageSwitchingTriggered("filepage"); });
    m_debugPageBtn.SetCallback([this]() { this->PageSwitchingTriggered("debugpage"); });
    m_downCompletePageBtn.SetCallback([this]() { this->PageSwitchingTriggered("completePage"); });
}

void SwitchPanel::InitCControls()
{
    m_downloadPageBtn.ModifyWndName("DownloadPage");
    m_debugPageBtn.ModifyWndName("DebugPage");
    m_downCompletePageBtn.ModifyWndName("CompletedPage");
}

void SwitchPanel::PageSwitchingTriggered(const std::string &pageId)
{
    m_OnPageSwitching.Emit(pageId);
}

void SwitchPanel::DoDataExchange(CDataExchange *pDX)
{
    CTabPage::DoDataExchange(pDX);
    auto controlExchange = [pDX](int nIDC, CWnd &wnd)
    { DDX_Control(pDX, nIDC, wnd); };

    controlExchange(SWITCH_PANEL_BTN1, m_downloadPageBtn);
    controlExchange(SWITCH_PANEL_BTN2, m_downCompletePageBtn);
    controlExchange(SWITCH_PANEL_BTN3, m_debugPageBtn);

    InitCControls();
}

