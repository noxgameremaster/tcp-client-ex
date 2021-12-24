
// fileDownloaderClientDlg.h: 헤더 파일
//

#pragma once

#include "logViewer.h"


// CfileDownloaderClientDlg 대화 상자
class CfileDownloaderClientDlg : public CDialogEx
{
private:
	LogViewer m_logViewer;

// 생성입니다.
public:
	CfileDownloaderClientDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILEDOWNLOADERCLIENT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

private:
	void Initialize();


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
