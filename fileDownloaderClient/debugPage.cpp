
#include "pch.h"
#include "debugPage.h"
#include "resource.h"

DebugPage::DebugPage(UINT nIDTemplate, CWnd *parent)
    : CTabPage(nIDTemplate, parent)
{ }

DebugPage::~DebugPage()
{ }

void DebugPage::NotifyTesting(const std::string &testName)
{
    m_OnTesting.Emit(testName, "");
}

void DebugPage::SendCommandLine()
{
    std::string cmd = m_textInput.GetTextString();

    if (cmd.empty())
        return;

    m_OnTesting.Emit("send command", cmd);
    m_textInput.Empty();
}

void DebugPage::OnInitialUpdate()
{
    m_btnEcho.SetCallback([this]() { this->NotifyTesting("echo test"); });
    m_btnFileReq.SetCallback([this]() { this->NotifyTesting("file request"); });
    m_btnInput.SetCallback([this]() { this->SendCommandLine(); });
}

void DebugPage::InitCControls()
{
    m_btnEcho.ModifyWndName("Echo Testing");
    m_btnFileReq.ModifyWndName("File Request");
    m_btnInput.ModifyWndName("Send");
}

void DebugPage::DoDataExchange(CDataExchange *pDX)
{
    CTabPage::DoDataExchange(pDX);
    auto controlExchange = [pDX](int nIDC, CWnd &wnd)
    { DDX_Control(pDX, nIDC, wnd); };

    controlExchange(DEBUG_EDIT, m_textInput);
    controlExchange(DEBUG_ECHO_BTN, m_btnEcho);
    controlExchange(DEBUG_FILE_REQ_BTN, m_btnFileReq);
    controlExchange(DEBUG_SEND_BTN, m_btnInput);

    InitCControls();
}
