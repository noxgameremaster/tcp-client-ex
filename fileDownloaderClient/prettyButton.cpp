
#include "pch.h"
#include "prettybutton.h"
#include "cbufferdc.h"

IMPLEMENT_DYNAMIC(PrettyButton, CMFCButton)

PrettyButton::PrettyButton()
    : CMFCButton()
{
}

PrettyButton::~PrettyButton()
{ }

void PrettyButton::DrawStuff(CDC &cdc)
{
    CBrush brush;
    CPen pen(PS_SOLID, 2, RGB(112, 146, 190));
    CPen *oldpen = cdc.SelectObject(&pen);
    CRect rect;

    cdc.SetBkMode(TRANSPARENT);
    //brush.CreateStockObject(NULL_BRUSH);
    brush.CreateSolidBrush(RGB(0, 162, 232));

    CBrush *oldBrush = cdc.SelectObject(&brush);

    GetClientRect(&rect);
    cdc.Rectangle(&rect);

    //int oldMode = SetBkMode(cdc, TRANSPARENT);

    cdc.SetTextColor(RGB(241, 241, 241));

    DrawText(cdc, m_name.c_str(), m_name.length(), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    //SetBkMode(cdc, oldMode);
    cdc.SelectObject(oldpen);
    cdc.SelectObject(oldBrush);
}

void PrettyButton::OnPaint()
{
    //CMFCButton::OnPaint();

    //CClientDC cdc(this);
    CBufferDC cdc(this);

    DrawStuff(cdc);
}

void PrettyButton::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_onClicked)
        m_onClicked();
}

void PrettyButton::PreSubclassWindow()
{
    CString name;

    GetWindowText(name);
    m_name = name;

    EnableWindowsTheming(false);
}

void PrettyButton::SetCallback(std::function<void()> &&onClicked)
{
    m_onClicked = onClicked;
}

void PrettyButton::ModifyWndName(const std::string &text)
{
    m_name = text;
}

BEGIN_MESSAGE_MAP(PrettyButton, CMFCButton)
    ON_WM_LBUTTONDOWN()
    ON_WM_PAINT()
END_MESSAGE_MAP()
