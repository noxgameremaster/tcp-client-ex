
#ifndef LOG_VIEWER_H__
#define LOG_VIEWER_H__

#include <list>
#include <memory>
#include <string>
#include <map>
#include <functional>

class LogViewer : public CListCtrl
{
    DECLARE_DYNAMIC(LogViewer)
    static constexpr int max_appear_slot_count = 8;
    struct LogDataAlive;
    class LogData;
    using log_data_ty = std::unique_ptr<LogData>;

    struct ViewerImpl;

private:
    std::list<log_data_ty> m_logdataList;
    std::list<log_data_ty>::const_iterator m_logdataPos;
    std::list<log_data_ty>::const_iterator m_logdataEndpos;

    std::map<std::string, std::function<bool()>> m_scrollMap;
    std::unique_ptr<ViewerImpl> m_viewImpl;

    std::weak_ptr<LogDataAlive> m_latestSelectSel;

public:
    explicit LogViewer();
    ~LogViewer() override;

//private:
    void UpdateViewer();
private:
    std::string CurrentLocalTime();

public:
    void CreateNewLog(const std::string &message);
    void CreateColumn(const std::string &columnName, const int &width);

private:
    bool UpViewer();
    bool PageUpViewer();
    bool DownViewer();
    bool PageDownViewer();
    LogData *GetLogData(int index);

public:
    void ViewerScrolling(const std::string &action);

protected:
    DECLARE_MESSAGE_MAP()
    BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult) override;
    BOOL PreTranslateMessage(MSG *pMsg);
    void PreSubclassWindow() override;
    afx_msg void OnPaint();
    void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif

