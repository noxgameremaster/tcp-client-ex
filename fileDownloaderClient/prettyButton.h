
#ifndef PRETTY_BUTTON_H__
#define PRETTY_BUTTON_H__

#include <functional>
#include <string>

class PrettyButton : public CMFCButton
{
    DECLARE_DYNAMIC(PrettyButton)

private:
    std::function<void()> m_onClicked;
    std::string m_name;

public:
    explicit PrettyButton();
    ~PrettyButton() override;

private:
    void DrawStuff(CDC &dc);

protected:
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

private:
    virtual void PreSubclassWindow() override;

public:
    void SetCallback(std::function<void()> &&onClicked);
    void ModifyWndName(const std::string &text);
};

#endif
