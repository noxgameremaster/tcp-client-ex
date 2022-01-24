
#ifndef TEXT_LABEL_H__
#define TEXT_LABEL_H__

#include <memory>

class TextLabel : public CStatic
{
private:
    std::unique_ptr<CBrush> m_bkBrush;
    UINT m_bkColor;
    UINT m_textColor;

public:
    explicit TextLabel();
    void SetBackgroundColor(UINT rgb);
    void SetTextColor(UINT rgb);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};

#endif
