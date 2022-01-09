
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
	GroupBox m_mainPanel;
	GroupBox m_logPanel;
	PrettyButton m_btnLogTestInsert;
	PrettyButton m_btnStartTest;
	PrettyButton m_btnFocusToEnd;
    PrettyButton m_btnReconnect;
	std::unique_ptr<CoreUi> m_coreUi;

	std::unique_ptr<MainWndCC> m_wndcc;
	std::unique_ptr<PageManager> m_logPanelLoader;


	//for test
	CEdit m_cmdInput;
	PrettyButton m_btnCmdOk;

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
	void SendInputCommand();

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg BOOL PreTranslateMessage(MSG *pMsg);
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
