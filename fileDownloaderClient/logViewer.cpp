
#include "pch.h"
#include "logViewer.h"
#include "stringHelper.h"

#include <chrono>

using namespace _StringHelper;

IMPLEMENT_DYNAMIC(LogViewer, CListCtrl)

static int s_columnIndex = 0;
static int s_increaseIndex = 0;

struct LogViewer::LogDataAlive
{
    LogData *m_parent;
};

class LogViewer::LogData
{
    friend LogViewer;

private:
    bool m_selected;
    int m_index;
    std::string m_message;
    std::string m_datetime;
    std::shared_ptr<LogDataAlive> m_alive;

public:
    LogData()
        : m_alive(new LogDataAlive)
    {
        m_alive->m_parent = this;
        m_selected = false;
        m_index = ++s_increaseIndex;
    }
};

struct LogViewer::ViewerImpl
{
    COLORREF m_bkColor;
    COLORREF m_foreColor;

public:
    ViewerImpl()
        : m_bkColor(RGB(255, 0, 255)), m_foreColor(RGB(0, 255, 0))
    { }
};

LogViewer::LogViewer()
    : CListCtrl(), m_viewImpl(new ViewerImpl)
{
    m_logdataPos = m_logdataList.cbegin();

    m_scrollMap.emplace("up", [this]() { return this->UpViewer(); });
    m_scrollMap.emplace("pageup", [this]() { return this->PageUpViewer(); });
    m_scrollMap.emplace("down", [this]() { return this->DownViewer(); });
    m_scrollMap.emplace("pagedown", [this]() { return this->PageDownViewer(); });
}

LogViewer::~LogViewer()
{ }

void LogViewer::UpdateViewer()
{
    SetRedraw(false);

    DeleteAllItems();
    std::list<log_data_ty>::const_iterator walkpos = m_logdataPos;
    int maxCount = max_appear_slot_count;
    int curCount = 0;

    while (walkpos != m_logdataList.cend() && maxCount--)
    {
        InsertItem(curCount, toArray(std::to_string(walkpos->get()->m_index)));
        SetItemText(curCount, 1, toArray(walkpos->get()->m_message));
        SetItemText(curCount, 2, toArray(walkpos->get()->m_datetime));
        ++curCount;
        ++walkpos;
    }
    SetRedraw(true);
}

std::string LogViewer::CurrentLocalTime()
{
    std::chrono::system_clock::time_point n = std::chrono::system_clock::now();

    std::time_t currentTime = std::chrono::system_clock::to_time_t(n);

    char buffer[256] = { };

    ctime_s(buffer, sizeof(buffer), &currentTime);
    return buffer;
}

void LogViewer::CreateNewLog(const std::string &message)
{
    log_data_ty log(new LogData);

    log->m_message = message;
    log->m_datetime = CurrentLocalTime();

    m_logdataList.push_back(std::move(log));
    if (m_logdataPos == m_logdataList.cend())
        m_logdataPos = m_logdataList.cbegin();
    m_logdataEndpos = m_logdataPos;

    int count = max_appear_slot_count;

    while (m_logdataEndpos != m_logdataList.cend() && count--)
        ++m_logdataEndpos;
}

void LogViewer::CreateColumn(const std::string &columnName, const int &width)
{
    InsertColumn(s_columnIndex++, toArray(columnName), LVCFMT_LEFT | LVCFMT_FIXED_WIDTH, width);
}

bool LogViewer::UpViewer()
{
    if (m_logdataPos == m_logdataList.cbegin())
        return false;

    --m_logdataPos;

    if (m_logdataEndpos == m_logdataList.cbegin())
        return false;

    --m_logdataEndpos;
    return true;
}

bool LogViewer::PageUpViewer()
{
    int count = max_appear_slot_count;

    while (m_logdataPos != m_logdataList.cbegin() && (count--))
    {
        --m_logdataPos;
        if (m_logdataEndpos == m_logdataList.cbegin())
            break;
        --m_logdataEndpos;
    }
    return count != max_appear_slot_count;
}

bool LogViewer::DownViewer()
{
    if (m_logdataEndpos == m_logdataList.cend())
        return false;

    ++m_logdataEndpos;

    if (m_logdataPos == m_logdataList.cend())
        return false;

    ++m_logdataPos;
    return true;
}

bool LogViewer::PageDownViewer()
{
    int count = max_appear_slot_count;

    while (m_logdataEndpos != m_logdataList.cend() && (count--))
    {
        ++m_logdataEndpos;
        if (m_logdataPos == m_logdataList.cend())
            break;
        ++m_logdataPos;
    }
    return count != max_appear_slot_count;
}

LogViewer::LogData *LogViewer::GetLogData(int index)
{
    if (index < 0)
        return nullptr;

    if (m_logdataPos == m_logdataList.cend())
        return nullptr;

    int count = index + 1;
    auto dataPos = m_logdataPos;

    while (count --)
    {
        ++dataPos;
        if (dataPos == m_logdataList.cend())
        {
            --dataPos;
            break;
        }
    }

    return dataPos->get();
}

void LogViewer::ViewerScrolling(const std::string &action)
{
    auto scrollIterator = m_scrollMap.find(action);

    if (scrollIterator == m_scrollMap.cend())
        return;

    if (scrollIterator->second())
        UpdateViewer();
}

BEGIN_MESSAGE_MAP(LogViewer, CListCtrl)
    ON_WM_PAINT()
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
END_MESSAGE_MAP()

BOOL LogViewer::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult)
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

BOOL LogViewer::PreTranslateMessage(MSG *pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
        return TRUE;

    return CListCtrl::PreTranslateMessage(pMsg);
}

void LogViewer::PreSubclassWindow()
{
    SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

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

void LogViewer::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
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
        OutputDebugString(toArray(std::to_string(pNMCD->dwItemSpec)));
        LogData *logData = GetLogData(pNMCD->dwItemSpec);

        if (logData == nullptr)
            break;
        if (GetItemState(pNMCD->dwItemSpec, LVIS_FOCUSED | LVIS_SELECTED) == (LVIS_FOCUSED | LVIS_SELECTED))
        {
            if (!m_latestSelectSel.expired())
            {
                auto prevSel = m_latestSelectSel.lock();

                prevSel->m_parent->m_selected = false;
            }
            m_latestSelectSel = logData->m_alive;
            logData->m_selected = true;
        }
        if (logData->m_selected)
        {
            pNMCD->uItemState = CDIS_DEFAULT;
            pLVCD->clrText = m_viewImpl->m_foreColor;
            pLVCD->clrTextBk = m_viewImpl->m_bkColor;
        }
    }
    break;

    default:
        *pResult = CDRF_DODEFAULT;
        break;
    }
}