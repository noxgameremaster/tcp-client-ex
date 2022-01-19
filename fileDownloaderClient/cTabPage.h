
#ifndef C_TAB_PAGE_H__
#define C_TAB_PAGE_H__

#include "ccobject.h"
#include <string>

class CTabPage : public CFormView, public CCObject
{
    DECLARE_DYNAMIC(CTabPage)
private:
    UINT m_nIDTemplate;
    CBrush m_bkBrush;
    UINT m_bkColor;
    std::string m_pageName;

protected:
    CWnd *m_parentWnd;

public:
    CTabPage(UINT _nIDTemplate, CWnd *parent = nullptr);
    virtual ~CTabPage();

private:
    void DrawStuff(CDC &cdc);

public:
    void ChangeBkColor(UINT _color);
    virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, CCreateContext *pContext = nullptr);
    virtual void OnInitialUpdate();
    virtual void OnEnterScreen();
    virtual void OnExitScreen();
    void Show(bool show = true);

    virtual bool CreatePage(const CRect &location, CCreateContext *pContext = nullptr);

    void SetPageName(const std::string &pageName)
    {
        m_pageName = pageName;
    }
    virtual void BeforeDestroy() {}

protected:
    virtual void DoDataExchange(CDataExchange *pDX) override;
    afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
    afx_msg void OnPaint();

    //afx_msg 
    DECLARE_MESSAGE_MAP()
};

#endif

