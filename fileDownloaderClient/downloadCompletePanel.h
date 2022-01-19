
#ifndef DOWNLOAD_COMPLETE_PANEL_H__
#define DOWNLOAD_COMPLETE_PANEL_H__

#include "cTabPage.h"
#include "listviewer.h"
#include "textlabel.h"

class IniFileMan;
class ListElement;
class CompletedFileInfo;

class DownloadCompletePanel : public CTabPage
{
private:
    ListViewer m_completeList;
    TextLabel m_textTitle;
    std::unique_ptr<IniFileMan> m_completeIni;
    std::list<std::shared_ptr<ListElement>> m_completeTempList;

public:
    explicit DownloadCompletePanel(UINT nIDTemplate, CWnd *parent);
    ~DownloadCompletePanel() override;

private:
    void UpdateReportView();
    void InitialLoadIni();
    void InitCControls();
    void OnInitialUpdate() override;
    void OnEnterScreen() override;
    void BeforeDestroy() override;

public:
    void SlotAppendComplete(std::shared_ptr<CompletedFileInfo> addItem);

protected:
    void DoDataExchange(CDataExchange *pDX) override;
};

#endif

