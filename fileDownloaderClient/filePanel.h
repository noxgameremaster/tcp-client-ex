
#ifndef FILE_PANEL_H__
#define FILE_PANEL_H__

#include "cTabPage.h"
#include "listviewer.h"
#include "textlabel.h"
#include <map>

class DownloadFileInfo;

class FilePanel : public CTabPage
{
private:
    ListViewer m_fileView;
    TextLabel m_textTitle;
    std::map<std::string, int> m_keyMap;

public:
    explicit FilePanel(UINT nIDTemplate, CWnd *parent);
    ~FilePanel() override;

private:
    void InitCControls();
    void OnInitialUpdate() override;

public:
    void SlotFileListAppend(std::shared_ptr<DownloadFileInfo> &&updateItem);
    void SlotFileListErase(std::shared_ptr<DownloadFileInfo> &&delItem);

protected:
    DECLARE_MESSAGE_MAP()

    void DoDataExchange(CDataExchange *pDX) override;
};

#endif

