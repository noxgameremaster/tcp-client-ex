
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
    //CBrush brush;
    CPen pen(PS_SOLID, 2, RGB(106, 182, 196));
    CPen *oldpen = dc.SelectObject(&pen);
    CRect rect;

    dc.SetBkMode(TRANSPARENT);
    /*brush.CreateStockObject(NULL_BRUSH);

    CBrush *oldBrush = dc.SelectObject(&brush);*/

    GetClientRect(&rect);
    //rect.top += 6;
    dc.Rectangle(&rect);
    dc.SelectObject(oldpen);
    //dc.SelectObject(oldBrush);
}

BEGIN_MESSAGE_MAP(GroupBox, CStatic)
    ON_WM_PAINT()
    ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

void GroupBox::OnPaint()
{
    //CStatic::OnPaint();

    CBufferDC cdc(this);
    
    DrawStuff(cdc);
}

HBRUSH GroupBox::CtlColor(CDC *pDC, UINT nCtlColor)
{
    pDC->SetBkColor(RGB(251, 248, 239));

    return m_brush;
}