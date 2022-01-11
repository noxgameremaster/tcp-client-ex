
#include "pch.h"
#include "logViewer.h"
#include "cbufferdc.h"
#include "resource.h"
#include "loopThread.h"
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
    COLORREF m_textColor;
    COLORREF m_backgroundColor;
    std::string m_message;
    std::string m_datetime;
    std::shared_ptr<LogDataAlive> m_alive;

public:
    LogData()
        : m_alive(new LogDataAlive)
    {
        m_selected = false;
        m_alive->m_parent = this;
        m_index = ++s_increaseIndex;
        m_textColor = RGB(0, 0, 0);
        m_backgroundColor = RGB(255, 255, 255);
    }
};

LogViewer::LogViewer()
    : CListCtrl()
{
    m_logdataPos = m_logdataList.cbegin();

    m_scrollMap.emplace("up", [this]() { return this->UpViewer(); });
    m_scrollMap.emplace("pageup", [this]() { return this->PageUpViewer(); });
    m_scrollMap.emplace("down", [this]() { return this->DownViewer(); });
    m_scrollMap.emplace("pagedown", [this]() { return this->PageDownViewer(); });
    m_scrollMap.emplace("end", [this]() { return this->MoveToPageEnd(); });
    m_addMsg = 0;
    m_updateThread = std::make_unique<LoopThread>();
    m_goToEnd = true;

    m_uiUpdateData.reserve(max_appear_slot_count);
}

LogViewer::~LogViewer()
{
    StopLogViewThread();
}

void LogViewer::UpdateDataCopy()
{
    std::list<log_data_ty>::const_iterator walkpos = m_logdataPos;
    int maxCount = max_appear_slot_count;
    int curCount = 0;

    m_uiUpdateData.clear();
    while (walkpos != m_logdataList.cend() && maxCount--)
    {
        if (walkpos != m_logdataList.cend())
        {
            LogData *copyLog = new LogData(*(*walkpos));

            m_uiUpdateData.push_back(log_data_ty(copyLog));
            ++curCount;
            ++walkpos;
        }
    }
}

void LogViewer::UpdateViewer()
{
    int updateCount = static_cast<int>(m_uiUpdateData.size());
    int maxCount = max_appear_slot_count;
    int curCount = 0;

    while (curCount < maxCount)
    {
        if (curCount < updateCount)
        {
            if (GetItemState(curCount, LVIS_FOCUSED | LVIS_SELECTED))
                SetItemState(curCount, 0, LVIS_FOCUSED | LVIS_SELECTED);
            SetItemText(curCount, 0, toArray(std::to_string(m_uiUpdateData[curCount]->m_index)));
            SetItemText(curCount, 1, toArray(m_uiUpdateData[curCount]->m_message));
            SetItemText(curCount, 2, toArray(m_uiUpdateData[curCount]->m_datetime));
        }
        else
        {
            SetItemText(curCount, 0, "");
            SetItemText(curCount, 1, "");
            SetItemText(curCount, 2, "");
        }
        ++curCount;
    }
}

void LogViewer::ConditionalUpdateViewer(bool forceUpdate)
{
    SetRedraw(false);
    do
    {
        std::lock_guard<std::mutex> guard(m_lock);
        bool passCond = forceUpdate ? true : (std::distance(m_logdataPos, m_logdataList.cend()) <= max_appear_slot_count);
        {
            if (!passCond)
                break;

            std::lock_guard<std::recursive_mutex> uiLock(m_uiLock);
            UpdateDataCopy();
        }

        {
            std::unique_lock<std::recursive_mutex> uiLock(m_uiLock);
            UpdateViewer();
        }
    }
    while (false);
    SetRedraw(true);
}

bool LogViewer::IsUpdateItem() const
{
    return m_addMsg > 0;
}

bool LogViewer::UpdateThreadTask()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    m_addMsg = 0;
    if (m_goToEnd)
        ViewerScrolling("end");
    else
        ConditionalUpdateViewer();

    return true;
}

std::string LogViewer::CurrentLocalTime()
{
    std::chrono::system_clock::time_point n = std::chrono::system_clock::now();

    time_t currentTime = std::chrono::system_clock::to_time_t(n);

    char buffer[256] = { };

    ctime_s(buffer, sizeof(buffer), &currentTime);
    return buffer;
}

void LogViewer::CreateNewLog(const std::string &message, uint32_t msgColor)
{
    log_data_ty log(new LogData);

    log->m_message = message;
    log->m_datetime = CurrentLocalTime();
    log->m_textColor = msgColor;

    {
        std::lock_guard<std::mutex> guard(m_lock);

        m_logdataList.push_back(std::move(log));
        if (m_logdataPos == m_logdataList.cend())
            m_logdataPos = m_logdataList.cbegin();
        m_logdataEndpos = m_logdataPos;

        int count = max_appear_slot_count;

        while (m_logdataEndpos != m_logdataList.cend() && count--)
            ++m_logdataEndpos;
    }

    ++m_addMsg;
    m_updateThread->Notify();
}

void LogViewer::CreateColumn(const std::string &columnName, const int &width)
{    
    InsertColumn(s_columnIndex++, toArray(columnName), LVCFMT_LEFT, width);
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

bool LogViewer::MoveToPageEnd()
{
    if (m_logdataList.empty())
        return false;

    auto endPos = m_logdataList.cend();
    auto startPos = endPos;
    int count = max_appear_slot_count;

    while (startPos != m_logdataList.cbegin() && (count--))
        --startPos;
    m_logdataPos = startPos;
    m_logdataEndpos = endPos;
    return true;
}

LogViewer::LogData *LogViewer::GetCopyLogData(int index)
{
    LogData *log = nullptr;

    try
    {
        log = m_uiUpdateData.at(index).get();
    }
    catch (const std::out_of_range &oor)
    {
        OutputDebugString(oor.what());
        return nullptr;
    }
    return log;
}

void LogViewer::ViewerScrolling(const std::string &action)
{
    std::unique_ptr<std::lock_guard<std::mutex>> guard(new std::lock_guard<std::mutex>(m_lock));
    auto scrollIterator = m_scrollMap.find(action);

    if (scrollIterator == m_scrollMap.cend())
        return;

    if (scrollIterator->second())
    {
        guard.reset();
        ConditionalUpdateViewer(true);
    }
}

void LogViewer::SetFocusToEnd()
{
    m_goToEnd = true;
}

void LogViewer::StopLogViewThread()
{
    if (m_updateThread)
    {
        m_updateThread->Notify();
        m_updateThread->Shutdown();
    }
}

void LogViewer::DrawStuff(CDC &pDC)
{
    CRect rc;

    pDC.SetBkMode(TRANSPARENT);
    GetWindowRect(&rc);
    ScreenToClient(&rc);
    pDC.FillSolidRect(rc, /*::GetSysColor(COLOR_WINDOW)*/ RGB(255, 0, 0));
}

BEGIN_MESSAGE_MAP(LogViewer, CListCtrl)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDOWN()
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
    SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SINGLEROW | LVS_EX_DOUBLEBUFFER);

    CRect headerRect;
    auto headCtrl = GetHeaderCtrl();

    headCtrl->GetClientRect(&headerRect);

    CreateColumn("Index", 50);
    CreateColumn("message", 400);
    CreateColumn("datetime", 200);

    GetClientRect(&m_updateLocation);
    m_updateLocation.top = headerRect.bottom;
    m_updateThread->SetTaskFunction([this]() { return this->UpdateThreadTask(); });
    m_updateThread->SetWaitCondition([this]() { return this->IsUpdateItem(); });
    m_updateThread->Startup();

    int count = max_appear_slot_count;

    while (count--)
        InsertItem(count, "");

    headCtrl->LockWindowUpdate();
}

BOOL LogViewer::OnEraseBkgnd(CDC */*pDC*/)
{
    return TRUE;
}

void LogViewer::OnPaint()
{
    /*CBufferDC cdc(this);

    DrawStuff(cdc);*/

    CListCtrl::OnPaint();   //!FIXME//

    CClientDC(this);
}

void LogViewer::OnLButtonDown(UINT nFlags, CPoint point)
{
    CListCtrl::OnLButtonDown(nFlags, point);

    if (m_goToEnd)
        m_goToEnd = false;
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
        std::unique_lock<std::recursive_mutex> uiLock(m_uiLock);
        LogData *logData = GetCopyLogData(pNMCD->dwItemSpec);

        if (logData == nullptr)
            break;

        if (pNMCD->uItemState & (LVIS_SELECTED | LVIS_FOCUSED))
            logData->m_selected ^= true;

        if (logData->m_selected)
        {
            pNMCD->uItemState = CDIS_DEFAULT;
            pLVCD->clrText = RGB(0, 255, 0);
            pLVCD->clrTextBk = RGB(255, 0, 255);
        }
        else
        {
            pLVCD->clrText = logData->m_textColor;
            pLVCD->clrTextBk = logData->m_backgroundColor;
        }
        *pResult = CDRF_DODEFAULT;
        break;
    }
    }
}