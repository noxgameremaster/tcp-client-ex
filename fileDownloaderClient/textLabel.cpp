
#include "pch.h"
#include "TextLabel.h"

TextLabel::TextLabel()
    : CStatic(), m_textColor(RGB(0, 0, 0))
{
    SetBackgroundColor(RGB(255, 255, 255));
}

void TextLabel::SetBackgroundColor(UINT rgb)
{
    m_bkColor = rgb;
    
    m_bkBrush = std::make_unique<CBrush>( /*m_bkColor*/);
}

void TextLabel::SetTextColor(UINT rgb)
{
    m_textColor = rgb;
}

BEGIN_MESSAGE_MAP(TextLabel, CStatic)
    ON_WM_CTLCOLOR_REFLECT()
    ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

LRESULT TextLabel::OnSetText(WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = Default();
    CRect Rect;

    GetWindowRect(&Rect);
    GetParent()->ScreenToClient(&Rect);
    GetParent()->InvalidateRect(&Rect);
    GetParent()->UpdateWindow();
    return Result;
}

HBRUSH TextLabel::CtlColor(CDC* pDC, UINT nCtlColor)
{
    pDC->SetTextColor(m_textColor);
    pDC->SetBkMode(TRANSPARENT);

    return (HBRUSH)GetStockObject(NULL_BRUSH);
}

