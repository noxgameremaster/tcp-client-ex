
#ifndef TEXT_INPUT_H__
#define TEXT_INPUT_H__

#include <memory>
#include <functional>
#include <string>

class TextInput : public CEdit
{
private:
    std::unique_ptr<CBrush> m_bkBrush;
    UINT m_bkColor;
    UINT m_textColor;

public:
    TextInput();
    virtual ~TextInput() override;

    void SetBackgroundColor(UINT rgb);
    void SetTextColor(UINT rgb);

    int GetWindowTextNumber();
    int GetWindowTextNumber(std::function<int(int)> functor);
    void SetWindowTextNumber(int number);

    void Empty() {
        SetWindowText({ });
    }
    std::string GetTextString();

private:
    void PreSubclassWindow() override;
    void FitTextLengthWithControl();

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    afx_msg BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

};

#endif
