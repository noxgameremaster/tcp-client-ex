
#ifndef DOWNLOAD_COMPLETE_PANEL_H__
#define DOWNLOAD_COMPLETE_PANEL_H__

#include "cTabPage.h"
#include "listviewer.h"
#include "textlabel.h"

class DownloadCompletePanel : public CTabPage
{
private:
    ListViewer m_completeList;
    TextLabel m_textTitle;

public:
    explicit DownloadCompletePanel(UINT nIDTemplate, CWnd *parent);
    ~DownloadCompletePanel() override;

private:
    void InitCControls();
    void OnInitialUpdate() override;
    void OnEnterScreen() override;

protected:
    void DoDataExchange(CDataExchange *pDX) override;
};

#endif

