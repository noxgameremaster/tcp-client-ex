
#ifndef GROUP_BOX_H__
#define GROUP_BOX_H__

class GroupBox : public CStatic
{
    DECLARE_DYNAMIC(GroupBox)

private:
    CBrush m_brush;

public:
    explicit GroupBox();
    ~GroupBox();

private:
    virtual void PreSubclassWindow() override;
    void DrawStuff(CDC &dc);

protected:
    afx_msg void OnPaint();
    afx_msg HBRUSH CtlColor(CDC *pDC, UINT nCtlColor);
    DECLARE_MESSAGE_MAP()
};

#endif

