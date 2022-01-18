
// fileDownloaderClientDlg.h: 헤더 파일
//

#pragma once

#include "prettyButton.h"
#include "groupbox.h"

#include <memory>

class CoreUi;
class PageManager;

// CfileDownloaderClientDlg 대화 상자
class CfileDownloaderClientDlg : public CDialogEx
{
	class MainWndCC;
private:
	std::unique_ptr<CBrush> m_bkBrush;
	GroupBox m_mainPanel;
	GroupBox m_logPanel;
	std::unique_ptr<CoreUi> m_coreUi;

	std::unique_ptr<MainWndCC> m_wndcc;
	std::unique_ptr<PageManager> m_logPanelLoader;
	std::unique_ptr<PageManager> m_mainPageLoader;
    std::unique_ptr<PageManager> m_switchPanelLoader;

// 생성입니다.
public:
	CfileDownloaderClientDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	~CfileDownloaderClientDlg() override;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILEDOWNLOADERCLIENT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

public:
	void AppendLogViewMessage(const std::string &message, uint32_t colr);

private:
	void InitPageManager();
	void Initialize();
    void DoPageSwitching(const std::string &pageId);
    void DoTesting(const std::string &testName, const std::string &context);

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg BOOL PreTranslateMessage(MSG *pMsg);
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
