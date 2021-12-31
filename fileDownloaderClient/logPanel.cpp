
#include "pch.h"
#include "logPanel.h"
#include "resource.h"

LogPanel::LogPanel(UINT nIDTemplate, CWnd *parent)
    : CTabPage(nIDTemplate, parent)
{ }

LogPanel::~LogPanel()
{ }

void LogPanel::ViewerMoveScroll(const std::string &action)
{
    m_logViewer.ViewerScrolling(action);
}

void LogPanel::OnInitialUpdate()
{
    m_btnPageDown.SetCallback([this]() { this->ViewerMoveScroll("pagedown"); });
    m_btnDown.SetCallback([this]() { this->ViewerMoveScroll("down"); });
    m_btnUp.SetCallback([this]() { this->ViewerMoveScroll("up"); });
    m_btnPageUp.SetCallback([this]() { this->ViewerMoveScroll("pageup"); });
}

void LogPanel::InitCControls()
{
    m_btnPageUp.ModifyWndName("¡ã");
    m_btnUp.ModifyWndName("¡â");
    m_btnDown.ModifyWndName("¡ä");
    m_btnPageDown.ModifyWndName("¡å");
}

BEGIN_MESSAGE_MAP(LogPanel, CTabPage)
END_MESSAGE_MAP()

void LogPanel::DoDataExchange(CDataExchange *pDX)
{
    CTabPage::DoDataExchange(pDX);
    auto controlExchange = [pDX](int nIDC, CWnd &wnd)
    { DDX_Control(pDX, nIDC, wnd); };

    controlExchange(LOG_PANEL_PAGEUP, m_btnPageUp);
    controlExchange(LOG_PANEL_UP, m_btnUp);
    controlExchange(LOG_PANEL_DOWN, m_btnDown);
    controlExchange(LOG_PANEL_PAGEDOWN, m_btnPageDown);

    InitCControls();
}

