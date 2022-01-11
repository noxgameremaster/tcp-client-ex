
#ifndef LIST_HEADER_PANEL_H__
#define LIST_HEADER_PANEL_H__

class ListHeaderPanel : public CHeaderCtrl
{
    DECLARE_DYNAMIC(ListHeaderPanel)

public:
    ListHeaderPanel();
    ~ListHeaderPanel() override;

private:
    void DrawHeaderStuff(CDC &cdc, const CRect &area);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC *pDC);
};

#endif

