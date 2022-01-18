
#include "pch.h"
#include "CTabPage.h"
#include "cbufferdc.h"

IMPLEMENT_DYNAMIC(CTabPage, CFormView)

CTabPage::CTabPage(UINT _nIDTemplate, CWnd *parent)
    : CFormView(_nIDTemplate), m_nIDTemplate(_nIDTemplate), m_bkBrush(RGB(106, 182, 196)), m_bkColor(RGB(106, 182, 196))
{
    m_parentWnd = parent;
}

CTabPage::~CTabPage()
{ }

void CTabPage::DrawStuff(CDC &cdc)
{
    CPen pen(PS_SOLID, 2, RGB(128, 255, 255));
    CBrush brush(RGB(13, 107, 115));
    CPen *oldPen = cdc.SelectObject(&pen);
    CBrush *oldBrush = cdc.SelectObject(&brush);
    CRect border;

    GetClientRect(&border);
    cdc.Rectangle(&border);

    cdc.SelectObject(oldPen);
    cdc.SelectObject(oldBrush);
}

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

void CTabPage::OnPaint()
{
    CBufferDC cdc(this);

    DrawStuff(cdc);
}

BEGIN_MESSAGE_MAP(CTabPage, CFormView)
    //ON_WM_CTLCOLOR()
    ON_WM_PAINT()
END_MESSAGE_MAP()
