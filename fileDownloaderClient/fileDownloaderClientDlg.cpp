
// fileDownloaderClientDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "fileDownloaderClient.h"
#include "fileDownloaderClientDlg.h"
#include "afxdialogex.h"

#include "coreui.h"
#include "pageManager.h"
#include "logPanel.h"
#include "stringHelper.h"

#pragma comment(lib, "tcpCommonSource.lib")
#pragma comment(lib, "tcp_client_ex.lib")
#pragma comment(lib, "netSystemFile")

using namespace _StringHelper;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CfileDownloaderClientDlg::MainWndCC : public CCObject
{
private:
	CfileDownloaderClientDlg *m_parent;

public:
	MainWndCC(CfileDownloaderClientDlg *parent)
		: CCObject(), m_parent(parent)
	{ }

	~MainWndCC() override
	{ }

	void GetNetLogMessage(const std::string &msg, uint32_t colr)
	{
		m_parent->AppendLogViewMessage(msg, colr);
	}
};

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CfileDownloaderClientDlg 대화 상자



CfileDownloaderClientDlg::CfileDownloaderClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILEDOWNLOADERCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_wndcc = std::make_unique<MainWndCC>(this);
	m_coreUi = std::make_unique<CoreUi>();
}

CfileDownloaderClientDlg::~CfileDownloaderClientDlg()
{ }

void CfileDownloaderClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, LOG_VIEWER_INSERT, m_btnLogTestInsert);
	DDX_Control(pDX, LOG_VIEWER_TEST_START, m_btnStartTest);
	DDX_Control(pDX, LOG_VIEWER_FOCUS_TOEND, m_btnFocusToEnd);
    DDX_Control(pDX, LOG_VIEWER_RECONNECT, m_btnReconnect);
	DDX_Control(pDX, MAIN_PAGE_PANEL, m_mainPanel);
	DDX_Control(pDX, MAIN_LOG_PANEL, m_logPanel);

	DDX_Control(pDX, MAIN_INPUT, m_cmdInput);
	DDX_Control(pDX, MAIN_INPUT_OK, m_btnCmdOk);
}

void CfileDownloaderClientDlg::AppendLogViewMessage(const std::string &message, uint32_t color)
{
	//m_logViewer.CreateNewLog(message, color);

	OutputDebugString(toArray(message));
}

void CfileDownloaderClientDlg::InitPageManager()
{
	CWnd *logPanelFrame = GetDlgItem(MAIN_LOG_PANEL);

	m_logPanelLoader = std::make_unique<PageManager>(*logPanelFrame, this);

	std::unique_ptr<LogPanel> logView(new LogPanel(IDD_LOGVIEW_PANEL, this));

	m_coreUi->OnForwardMessage().Connection(&LogPanel::LogPanelRecv::SlotAddLog, logView->ReceiveObject());

	m_logPanelLoader->MakePage("logview", std::move(logView));
	m_logPanelLoader->ShowPage("logview");
}

void CfileDownloaderClientDlg::Initialize()
{
	m_coreUi->Initialize();
	m_coreUi->OnForwardMessage().Connection(&MainWndCC::GetNetLogMessage, m_wndcc.get());

	SetWindowText(toArray(std::string("Client Application")));

	InitPageManager();
	m_btnLogTestInsert.SetCallback([this]() { this->m_coreUi->DoTestEcho(); });
	m_btnStartTest.SetCallback([this]() { this->m_coreUi->DoTestFilePacket(); });
	m_btnCmdOk.SetCallback([this]() { this->SendInputCommand(); });

	m_btnLogTestInsert.ModifyWndName("echo test");
	m_btnStartTest.ModifyWndName("file req");
	m_btnFocusToEnd.ModifyWndName("go to end");

	m_btnCmdOk.ModifyWndName("send");
    m_btnReconnect.ModifyWndName("reconnect");

	m_coreUi->StartNetClient();
}

void CfileDownloaderClientDlg::SendInputCommand()
{
	CString cmd;

	m_cmdInput.GetWindowTextA(cmd);
	if (m_coreUi)
	{
		m_coreUi->SendCommandToServer(cmd.GetBuffer());
		m_cmdInput.SetWindowTextA("");
	}
}

BEGIN_MESSAGE_MAP(CfileDownloaderClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CfileDownloaderClientDlg 메시지 처리기

BOOL CfileDownloaderClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	Initialize();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CfileDownloaderClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CfileDownloaderClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

BOOL CfileDownloaderClientDlg::PreTranslateMessage(MSG *pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
		case VK_ESCAPE:
		case VK_F1:
			return TRUE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CfileDownloaderClientDlg::OnClose()
{
	ShowWindow(SW_HIDE);
	if (m_logPanelLoader)
	{
		m_logPanelLoader.reset();
	}

	//m_logViewer.StopLogViewThread();
	if (m_coreUi)
	{
		m_coreUi->Deinitialize();
		m_coreUi.reset();
	}
	OutputDebugString("shutdown app");
	CDialogEx::OnClose();
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CfileDownloaderClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

