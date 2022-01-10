
#ifndef FILE_PANEL_H__
#define FILE_PANEL_H__

#include "cTabPage.h"
#include "listviewer.h"

class FilePanel : public CTabPage
{
private:
    ListViewer m_fileView;

public:
    explicit FilePanel(UINT nIDTemplate, CWnd *parent);
    ~FilePanel() override;

private:
    void InitCControls();
    void OnInitialUpdate() override;

protected:
    DECLARE_MESSAGE_MAP()

    void DoDataExchange(CDataExchange *pDX) override;
};

#endif

