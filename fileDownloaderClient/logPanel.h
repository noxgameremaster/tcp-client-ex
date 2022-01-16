
#ifndef LOG_PANEL_H__
#define LOG_PANEL_H__

#include "ccobject.h"
#include "cTabPage.h"
#include "logViewer.h"
#include "prettyButton.h"

class LogPanel : public CTabPage
{
public:
	class LogPanelRecv : public CCObject
	{
	private:
		LogPanel *m_parent;

	public:
		LogPanelRecv(LogPanel *parent)
		{
			m_parent = parent;
		}
		~LogPanelRecv() override
		{ }

	public:
		void SlotAddLog(const std::string &msg, uint32_t colr)
		{
			m_parent->m_logViewer.CreateNewLog(msg, colr);
		}
	};
private:
	friend LogPanelRecv;

	LogViewer m_logViewer;
	PrettyButton m_btnPageUp;
	PrettyButton m_btnUp;
	PrettyButton m_btnDown;
	PrettyButton m_btnPageDown;
	PrettyButton m_btnEndFocus;
	std::unique_ptr<LogPanelRecv> m_panelRecv;

public:
	explicit LogPanel(UINT nIDTemplate, CWnd *parent);
	~LogPanel() override;

private:
	void ViewerMoveScroll(const std::string &action);
	void OnInitialUpdate() override;
	void InitCControls();
	void FocusEndline();
    void BeforeDestroy() override;

public:
	LogPanelRecv *ReceiveObject()
	{
		return m_panelRecv.get();
	}

protected:
	DECLARE_MESSAGE_MAP()

	void DoDataExchange(CDataExchange *pDX) override;
};

#endif

