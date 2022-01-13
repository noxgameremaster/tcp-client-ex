
#include "pch.h"
#include "listviewer.h"
#include "listElement.h"
#include "listHeaderPanel.h"
#include "cbufferdc.h"
#include "mymemdc.h"
#include "stringHelper.h"
#include "resource.h"

using namespace _StringHelper;

IMPLEMENT_DYNAMIC(ListViewer, CListCtrl)

ListViewer::ListViewer()
    : CListCtrl()
{
    m_headerPanel = std::make_unique<ListHeaderPanel>();
}

ListViewer::~ListViewer()
{ }

void ListViewer::DrawStuff(CDC &cdc)
{
    CPen pen(PS_SOLID, 2, RGB(255, 0, 0));
    CPen *oldPen = cdc.SelectObject(&pen);
    CRect border;

    GetWindowRect(&border);
    //ScreenToClient(&border);

    cdc.Rectangle(&border);

    cdc.SelectObject(oldPen);
}

bool ListViewer::GetListData(int keyId, ListViewer::list_element_ty &dest)
{
    auto keyIterator = m_datamap.find(keyId);

    if (keyIterator == m_datamap.cend())
        return false;

    dest = keyIterator->second;
    return true;
}

bool ListViewer::UpdateChangedCell(list_element_ty &updateData)
{
    int cellId = 0;

    for (const auto &item : m_listdata)
    {
        if (item == updateData)
        {
            RedrawItems(cellId, cellId);
            return true;
        }
        ++cellId;
    }
    return false;
}

void ListViewer::Append(int keyId, list_element_ty addData)
{
    list_element_ty search;

    if (GetListData(keyId, search)) //replace from old stuff
    {
        //*search = *addData; //need virtual clone here
        search->Clone(addData.get());
        UpdateChangedCell(search);
        return;
    }
    list_element_ty moving = std::move(addData);

    m_listdata.push_back(moving);
    SetItemCount(m_listdata.size());
    m_datamap.emplace(keyId, moving);
}

void ListViewer::AttachListColumn(const ListColumn &columnData)
{
    int col = 0;

    for (const auto &column : columnData.m_columns)
        InsertColumn(col++, toArray(std::get<0>(column)), LVCFMT_LEFT | LVCFMT_FIXED_WIDTH, std::get<1>(column));
}

void ListViewer::EnableHighlighting(HWND hWnd, int row, bool bHighlight)
{
    ListView_SetItemState(hWnd, row, bHighlight ? 0xff : 0, LVIS_SELECTED);
}

bool ListViewer::IsRowSelected(HWND hWnd, int row)
{
    return ListView_GetItemState(hWnd, row, LVIS_SELECTED) != 0;
}

bool ListViewer::IsRowHighlighted(HWND hWnd, int row)
{
    return IsRowSelected(hWnd, row) /*&& (::GetFocus() == hWnd)*/;
}

BEGIN_MESSAGE_MAP(ListViewer, CListCtrl)
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDisplayInfoList)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
    ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL ListViewer::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
    switch (((NMHDR *)lParam)->code)
    {
    case HDN_BEGINTRACKW:
    case HDN_BEGINTRACKA:
        *pResult = TRUE;

        return TRUE;
    }

    return CListCtrl::OnNotify(wParam, lParam, pResult);
}

void ListViewer::OnGetDisplayInfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LV_DISPINFO *pDispInfo = reinterpret_cast<LV_DISPINFO *>(pNMHDR);
    LV_ITEM *pItem = &(pDispInfo->item);
    int itemId = pItem->iItem;

    if (pItem->mask & LVIF_TEXT)
    {
        std::string text = m_listdata[itemId]->GetElement(pItem->iSubItem);

        if (text.size())
            lstrcpyn(pItem->pszText, text.c_str(), pItem->cchTextMax);
    }
    if (pItem->mask & LVIF_IMAGE)
    { }

    pResult[0] = 0;
    //RedrawItems()
}

void ListViewer::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    NMTVCUSTOMDRAW *pLVCD = reinterpret_cast<NMTVCUSTOMDRAW *>(pNMHDR);
    static bool bHighlighted = false;

    *pResult = CDRF_DODEFAULT;
    if (CDDS_PREPAINT == pNMCD->dwDrawStage)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if (CDDS_ITEMPREPAINT == pNMCD->dwDrawStage)
    {
        int iRow = static_cast<int>(pNMCD->dwItemSpec);

        bHighlighted = IsRowHighlighted(m_hWnd, iRow);

        if (bHighlighted)
        {
            pLVCD->clrText = RGB(32, 194, 44); // Use my foreground hilite color
            pLVCD->clrTextBk = RGB(192, 0, 208); // Use my background hilite color

            EnableHighlighting(m_hWnd, iRow, false);
        }
        else
            pLVCD->clrText = RGB(231, 231, 248);

        *pResult = CDRF_DODEFAULT | CDRF_NOTIFYPOSTPAINT;
    }
    else if (CDDS_ITEMPOSTPAINT == pNMCD->dwDrawStage)
    {
        if (bHighlighted)
        {
            int iRow = static_cast<int>(pNMCD->dwItemSpec);

            EnableHighlighting(m_hWnd, iRow, true);
        }
        *pResult = CDRF_DODEFAULT;
    }
}

void ListViewer::OnPaint()
{
    /*CListCtrl::OnPaint();
    CBufferDC cdc(this);

    DrawStuff(cdc);*/

    CPaintDC dc(this);
    CRect rect;
    GetClientRect(&rect);
    MyMemDC memDC(&dc, rect);
    CRect headerRect;

    GetDlgItem(0)->GetWindowRect(&headerRect);
    ScreenToClient(&headerRect);
    dc.ExcludeClipRect(&headerRect);
    
    CRect clip;
    memDC.GetClipBox(&clip);
    memDC.FillSolidRect(clip, RGB(76, 85, 118));

    SetTextBkColor(RGB(76, 85, 118));

    /*m_SkinVerticleScrollbar.UpdateThumbPosition();
    m_SkinHorizontalScrollbar.UpdateThumbPosition();*/


    DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
}

void ListViewer::PreSubclassWindow()
{
    SetExtendedStyle(LVS_EX_FULLROWSELECT | /*LVS_EX_GRIDLINES |*/ LVS_EX_DOUBLEBUFFER);

    CHeaderCtrl *pHeadCtrl = GetHeaderCtrl();
    HWND hWnd = reinterpret_cast<HWND>(::SendMessage(m_hWnd, LVM_GETHEADER, 0, 0));

    pHeadCtrl->ModifyStyle(0, LVS_OWNERDRAWFIXED, SWP_FRAMECHANGED);
    m_headerPanel->SubclassWindow(hWnd);

    CListCtrl::PreSubclassWindow();
}

