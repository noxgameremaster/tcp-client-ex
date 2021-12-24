
#include "pch.h"
#include "logViewer.h"

IMPLEMENT_DYNAMIC(LogViewer, CListCtrl)

LogViewer::LogViewer()
    : CListCtrl()
{ }

LogViewer::~LogViewer()
{
}

void LogViewer::PreSubclassWindow()
{ }

void LogViewer::OnPaint()
{
    CListCtrl::OnPaint();
    CClientDC cdc(this);
    CBrush brush;
    CPen pen(PS_SOLID, 2, RGB(112, 146, 190));
    CPen *oldpen = cdc.SelectObject(&pen);
    CRect rect;

    brush.CreateStockObject(NULL_BRUSH);
    //brush.CreateSolidBrush(RGB(0, 162, 232));

    CBrush *oldBrush = cdc.SelectObject(&brush);

    GetClientRect(&rect);
    cdc.Rectangle(&rect);

    int oldMode = SetBkMode(cdc, TRANSPARENT);

    cdc.SetTextColor(RGB(241, 241, 241));

    //DrawText(cdc, m_name.c_str(), m_name.length(), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SetBkMode(cdc, oldMode);
    cdc.SelectObject(oldpen);
    cdc.SelectObject(oldBrush);
}

BEGIN_MESSAGE_MAP(LogViewer, CListCtrl)
    ON_WM_PAINT()
END_MESSAGE_MAP()