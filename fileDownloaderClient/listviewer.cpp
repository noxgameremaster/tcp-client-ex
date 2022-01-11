
#include "pch.h"
#include "listviewer.h"
#include "listElement.h"
#include "stringHelper.h"
#include "resource.h"

using namespace _StringHelper;

IMPLEMENT_DYNAMIC(ListViewer, CListCtrl)

ListViewer::ListViewer()
    : CListCtrl()
{ }

ListViewer::~ListViewer()
{ }

void ListViewer::Append(list_element_ty addData)
{
    m_listdata.push_back(std::move(addData));

    SetItemCount(m_listdata.size());
}

void ListViewer::AttachListColumn(const ListColumn &columnData)
{
    int col = 0;

    for (const auto &column : columnData.m_columns)
        InsertColumn(col++, toArray(std::get<0>(column)), LVCFMT_LEFT, std::get<1>(column));
}

BEGIN_MESSAGE_MAP(ListViewer, CListCtrl)
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDisplayInfoList)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
END_MESSAGE_MAP()

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
}

void ListViewer::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    NMTVCUSTOMDRAW *pLVCD = reinterpret_cast<NMTVCUSTOMDRAW *>(pNMHDR);

    switch (pNMCD->dwDrawStage)
    {
    case CDDS_PREPAINT:
        // Item prepaint notification.
        *pResult = CDRF_NOTIFYITEMDRAW;
        break;

    case CDDS_ITEMPREPAINT:
    {
        /*std::unique_lock<std::recursive_mutex> uiLock(m_uiLock);
        LogData *logData = GetCopyLogData(pNMCD->dwItemSpec);

        if (logData == nullptr)
            break;*/

        /*if (pNMCD->uItemState & (LVIS_SELECTED | LVIS_FOCUSED))
            logData->m_selected ^= true;

        if (logData->m_selected)
        {*/
            pNMCD->uItemState = CDIS_DEFAULT;
            pLVCD->clrText = RGB(0, 255, 0);
            pLVCD->clrTextBk = RGB(255, 0, 255);
        //}
        //else
        /*{
            pLVCD->clrText = logData->m_textColor;
            pLVCD->clrTextBk = logData->m_backgroundColor;
        }*/
        *pResult = CDRF_DODEFAULT;
        break;
    }
    }
}

void ListViewer::PreSubclassWindow()
{
    SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
}

