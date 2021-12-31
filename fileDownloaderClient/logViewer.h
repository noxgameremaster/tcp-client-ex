
#ifndef LOG_VIEWER_H__
#define LOG_VIEWER_H__

#include <list>
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <mutex>
#include <atomic>

class LoopThread;

class LogViewer : public CListCtrl
{
    DECLARE_DYNAMIC(LogViewer)
    static constexpr int max_appear_slot_count = 9;
    struct LogDataAlive;
    class LogData;
    using log_data_ty = std::unique_ptr<LogData>;

private:
    std::list<log_data_ty> m_logdataList;
    std::list<log_data_ty>::const_iterator m_logdataPos;
    std::list<log_data_ty>::const_iterator m_logdataEndpos;

    std::map<std::string, std::function<bool()>> m_scrollMap;

    std::weak_ptr<LogDataAlive> m_latestSelectCell;
    CRect m_updateLocation;

    std::atomic<int> m_addMsg;
    std::unique_ptr<LoopThread> m_updateThread;
    bool m_goToEnd;

public:
    explicit LogViewer();
    ~LogViewer() override;

public:
    void UpdateViewer();
    void ConditionalUpdateViewer();
    bool IsUpdateItem() const;
    void UpdateThreadTask();

private:
    std::string CurrentLocalTime();

public:
    void CreateNewLog(const std::string &message, uint32_t msgColor = 0);
    void CreateColumn(const std::string &columnName, const int &width);

private:
    bool UpViewer();
    bool PageUpViewer();
    bool DownViewer();
    bool PageDownViewer();
    bool MoveToPageEnd();
    LogData *GetLogData(int index);

public:
    void ViewerScrolling(const std::string &action);
    void SetFocusToEnd();
    void StopLogViewThread();

private:
    void DrawStuff(CDC &pDC);

protected:
    DECLARE_MESSAGE_MAP()
    BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult) override;
    BOOL PreTranslateMessage(MSG *pMsg);
    void PreSubclassWindow() override;
    BOOL OnEraseBkgnd(CDC *pDC);
    afx_msg void OnPaint();
    void OnLButtonDown(UINT, CPoint);
    void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);

private:
    std::mutex m_lock;
};

#endif

