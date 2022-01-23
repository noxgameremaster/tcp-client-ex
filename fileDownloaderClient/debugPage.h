
#ifndef DEBUG_PAGE_H__
#define DEBUG_PAGE_H__

#include "cTabPage.h"
#include "prettyButton.h"
#include "textInput.h"
#include "textLabel.h"
#include "ccobject.h"

class DebugPage : public CTabPage
{
private:
    PrettyButton m_btnInput;
    PrettyButton m_btnEcho;
    PrettyButton m_btnFileReq;
    PrettyButton m_btnReconn;
    TextInput m_textInput;
    TextLabel m_textTitle;

public:
    DebugPage(UINT nIDTemplate, CWnd *parent);
    ~DebugPage() override;

private:
    void NotifyTesting(const std::string &testName);
    void SendCommandLine();
    void OnInitialUpdate() override;
    void InitCControls();

protected:
    void DoDataExchange(CDataExchange *pDX) override;

public:
    DECLARE_SIGNAL(OnTesting, std::string, std::string)
};

#endif

