
#ifndef LOG_VIEWER_H__
#define LOG_VIEWER_H__

class LogViewer : public CListCtrl
{
    DECLARE_DYNAMIC(LogViewer)

public:
    explicit LogViewer();
    ~LogViewer() override;

protected:
    void PreSubclassWindow() override;
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

#endif

