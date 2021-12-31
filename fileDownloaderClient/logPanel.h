
#ifndef LOG_PANEL_H__
#define LOG_PANEL_H__

#include "cTabPage.h"
#include "logViewer.h"
#include "prettyButton.h"

class LogPanel : public CTabPage
{
private:
	LogViewer m_logViewer;
	PrettyButton m_btnPageUp;
	PrettyButton m_btnUp;
	PrettyButton m_btnDown;
	PrettyButton m_btnPageDown;

public:
	explicit LogPanel(UINT nIDTemplate, CWnd *parent);
	~LogPanel() override;

private:
	void ViewerMoveScroll(const std::string &action);
	void OnInitialUpdate() override;
	void InitCControls();

protected:
	DECLARE_MESSAGE_MAP()

	void DoDataExchange(CDataExchange *pDX) override;
};

#endif

