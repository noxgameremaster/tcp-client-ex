
#ifndef SWITCH_PANEL_H__
#define SWITCH_PANEL_H__

#include "cTabPage.h"
#include "prettyButton.h"
#include "ccobject.h"

class SwitchPanel : public CTabPage
{
private:
    PrettyButton m_downloadPageBtn;
    PrettyButton m_downCompletePageBtn;
    PrettyButton m_debugPageBtn;

public:
    SwitchPanel(UINT nIDTemplate, CWnd *parent);
    ~SwitchPanel() override;

private:
    void OnInitialUpdate() override;
    void InitCControls();
    void PageSwitchingTriggered(const std::string &pageId);

protected:
    void DoDataExchange(CDataExchange *pDX) override;

public:
    DECLARE_SIGNAL(OnPageSwitching, std::string)
};

#endif

