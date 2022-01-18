
#include "pch.h"
#include "TextInput.h"

#include <string>

TextInput::TextInput()
    : CEdit(), m_textColor(RGB(0, 0, 0))
{ }

TextInput::~TextInput()
{ }

void TextInput::SetBackgroundColor(UINT rgb)
{
    m_bkColor = rgb;
    m_bkBrush = std::make_unique<CBrush>(m_bkColor);
}

void TextInput::SetTextColor(UINT rgb)
{
    m_textColor = rgb;
}

int TextInput::GetWindowTextNumber()
{
    CString numberString;

    GetWindowText(numberString);
    return _ttoi(numberString);
}

int TextInput::GetWindowTextNumber(std::function<int(int)> functor)
{
    CString numberString;

    GetWindowText(numberString);
    return functor(_ttoi(numberString));
}

void TextInput::SetWindowTextNumber(int number)
{
    std::string numberString = std::to_string(number);

    SetWindowText(CString(numberString.c_str()));
}

std::string TextInput::GetTextString()
{
    CString textString;

    GetWindowText(textString);
    return textString.GetBuffer();
}

void TextInput::PreSubclassWindow()
{
    SetBackgroundColor(RGB(255, 255, 255));
    FitTextLengthWithControl();
}

void TextInput::FitTextLengthWithControl()
{
    TEXTMETRIC tm;
    CDC *pDC = GetDC();
    CRect rect;

    pDC->GetTextMetrics(&tm);
    ReleaseDC(pDC);
    GetRect(&rect);
    LimitText(rect.Width() / tm.tmAveCharWidth);
}

BEGIN_MESSAGE_MAP(TextInput, CEdit)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


HBRUSH TextInput::CtlColor(CDC* pDC, UINT nCtlColor)
{
    pDC->SetTextColor(m_textColor);
    pDC->SetBkColor(m_bkColor);

    return *m_bkBrush;
}

BOOL TextInput::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if (pMsg->message == WM_KEYDOWN)
    {
        BOOL b = GetKeyState(VK_CONTROL) & 0x80;
        if (b && (pMsg->wParam == 'v' || pMsg->wParam == 'V'))
        {
            SetSel(0, -1);
            return TRUE;
        }
    }

    return CEdit::PreTranslateMessage(pMsg);
}

void TextInput::OnContextMenu(CWnd* pWnd, CPoint point)
{ }
