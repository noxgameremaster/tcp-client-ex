
#include "pch.h"
#include "groupbox.h"
#include "cbufferdc.h"

IMPLEMENT_DYNAMIC(GroupBox, CStatic)

GroupBox::GroupBox()
    : CStatic(), m_brush(RGB(250, 250, 250))
{
}

GroupBox::~GroupBox()
{ }

void GroupBox::PreSubclassWindow()
{
    SetWindowText("");
}

void GroupBox::DrawStuff(CDC &dc)
{
    CPen pen(PS_SOLID, 2, RGB(106, 182, 196));
    CPen *oldpen = dc.SelectObject(&pen);
    CRect rect;

    dc.SetBkMode(TRANSPARENT);

    GetClientRect(&rect);
    dc.Rectangle(&rect);
    dc.SelectObject(oldpen);
}

BEGIN_MESSAGE_MAP(GroupBox, CStatic)
    ON_WM_PAINT()
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void GroupBox::OnPaint()
{
    CBufferDC cdc(this);
    
    DrawStuff(cdc);
}

BOOL GroupBox::OnEraseBkgnd(CDC */*pDC*/)
{
    return TRUE;
}

HBRUSH GroupBox::CtlColor(CDC *pDC, UINT nCtlColor)
{
    pDC->SetBkColor(RGB(251, 248, 239));

    return m_brush;
}