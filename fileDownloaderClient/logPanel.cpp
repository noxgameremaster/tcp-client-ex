
#include "pch.h"
#include "logPanel.h"
#include "resource.h"

LogPanel::LogPanel(UINT nIDTemplate, CWnd *parent)
    : CTabPage(nIDTemplate, parent)
{
    m_panelRecv = std::make_unique<LogPanelRecv>(this);
}

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
    m_btnEndFocus.SetCallback([this]() { this->FocusEndline(); });
}

void LogPanel::InitCControls()
{
    m_btnPageUp.ModifyWndName("¡ã");
    m_btnUp.ModifyWndName("¡â");
    m_btnDown.ModifyWndName("¡ä");
    m_btnPageDown.ModifyWndName("¡å");
    m_btnEndFocus.ModifyWndName("End");
}

void LogPanel::FocusEndline()
{
    m_logViewer.SetFocusToEnd();
}

void LogPanel::BeforeDestroy()
{
    m_logViewer.StopLogViewThread();
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
    controlExchange(LOG_PANEL_VIEW, m_logViewer);
    controlExchange(LOG_PANEL_END_FOCUS, m_btnEndFocus);

    InitCControls();
}

