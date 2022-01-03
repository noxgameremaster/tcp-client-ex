
#include "pch.h"
#include "CTabPage.h"

IMPLEMENT_DYNAMIC(CTabPage, CFormView)

CTabPage::CTabPage(UINT _nIDTemplate, CWnd *parent)
    : CFormView(_nIDTemplate), m_nIDTemplate(_nIDTemplate), m_bkBrush(RGB(106, 182, 196)), m_bkColor(RGB(106, 182, 196))
{
    m_parentWnd = parent;
}

CTabPage::~CTabPage()
{ }

void CTabPage::ChangeBkColor(UINT _color)
{
    m_bkColor = _color;
    m_bkBrush.DeleteObject();
    m_bkBrush.CreateSolidBrush(_color);
}

BOOL CTabPage::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, CCreateContext *pContext)
{
    //Todo something...
    return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, m_nIDTemplate, pContext);
}

void CTabPage::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    //Todo: init this dialog here
}

//@brief. 해당 페이지 입장
void CTabPage::OnEnterScreen()
{ }

//@brief. 해당 페이지로 부터 나감
void CTabPage::OnExitScreen()
{ }

void CTabPage::Show(bool show)
{
    if (show)
        OnEnterScreen();
    else
        OnExitScreen();
    ShowWindow(show ? SW_SHOW : SW_HIDE);
}

bool CTabPage::CreatePage(const CRect &location, CCreateContext *pContext)
{
    bool result = Create(nullptr, nullptr, WS_CHILD | WS_VSCROLL | WS_HSCROLL, location, m_parentWnd, pContext) & true;

    if (result)
        UpdateData(false);

    return result;
}

void CTabPage::DoDataExchange(CDataExchange *pDX)
{
    CFormView::DoDataExchange(pDX);
}

HBRUSH CTabPage::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
    CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

    pDC->SetBkColor(m_bkColor);
    return m_bkBrush;
}

BEGIN_MESSAGE_MAP(CTabPage, CFormView)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()
