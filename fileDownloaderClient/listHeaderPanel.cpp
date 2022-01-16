
#include "pch.h"
#include "listHeaderPanel.h"
#include "cbufferdc.h"

static constexpr UINT header_ctrl_first_color = RGB(239, 237, 248);
static constexpr UINT header_ctrl_second_color = RGB(155, 139, 211);
static constexpr UINT header_ctrl_text_color = RGB(16, 32, 64);

IMPLEMENT_DYNAMIC(ListHeaderPanel, CHeaderCtrl)

ListHeaderPanel::ListHeaderPanel()
    : CHeaderCtrl()
{ }

ListHeaderPanel::~ListHeaderPanel()
{ }

void ListHeaderPanel::DrawHeaderStuff(CDC &cdc, const CRect &area)
{
    TRIVERTEX triver[2];

    triver[0].x = area.left;
    triver[0].y = area.top;
    triver[0].Red = GetRValue(header_ctrl_first_color) << 8;
    triver[0].Green = GetGValue(header_ctrl_first_color) << 8;
    triver[0].Blue = GetBValue(header_ctrl_first_color) << 8;
    triver[0].Alpha = 0;

    //End Gradient Info
    triver[1].x = area.right;
    triver[1].y = area.bottom;
    triver[1].Red = GetRValue(header_ctrl_second_color) << 8;
    triver[1].Green = GetGValue(header_ctrl_second_color) << 8;
    triver[1].Blue = GetBValue(header_ctrl_second_color) << 8;
    triver[1].Alpha = 0;

    GRADIENT_RECT gRt = { 0,1 };

    GradientFill(cdc.m_hDC, triver, 2, &gRt, 1, GRADIENT_FILL_RECT_V);

    /*CPen pen(PS_SOLID, 1, RGB(24, 52, 76));
    CPen* pOld = cdc.SelectObject(&pen);*/
}

BEGIN_MESSAGE_MAP(ListHeaderPanel, CHeaderCtrl)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void ListHeaderPanel::OnPaint()
{
    CBufferDC cdc(this);

    int nItemCount = GetItemCount();
    int nStart = 0;

    for (int u = 0 ; u < nItemCount ; ++u)
    {
        CRect area;
        HDITEM item = { };
        TCHAR strBuffer[256] = { };

        item.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER | HDI_WIDTH;
        item.pszText = strBuffer;
        item.cchTextMax = sizeof(strBuffer)-1;

        GetItemRect(u, &area);
        GetItem(u, &item);

        area.left = nStart;
        area.right = area.left + item.cxy;
        nStart = area.right;

        DrawHeaderStuff(cdc, area);
        cdc.SetBkMode(TRANSPARENT);
        cdc.SelectObject(GetFont());
        cdc.SetTextColor(header_ctrl_text_color);
        cdc.MoveTo(area.right - 1, area.top);
        cdc.LineTo(area.right - 1, area.bottom);
        if (item.pszText)
        {
            area.left += 5;
            area.top += 2;
            cdc.DrawText(CString(item.pszText), area, DT_SINGLELINE | DT_WORD_ELLIPSIS);
        }
    }
}

BOOL ListHeaderPanel::OnEraseBkgnd(CDC *pDC)
{
    CRect rt;

    pDC->GetClipBox(rt);
    DrawHeaderStuff(*pDC, rt);

    return TRUE;//CHeaderCtrl::OnEraseBkgnd(pDC);
}