
#ifndef FILE_PANEL_H__
#define FILE_PANEL_H__

#include "cTabPage.h"
#include "listviewer.h"
#include "textlabel.h"
#include "ccobject.h"
#include <map>
#include <stack>

class DownloadFileInfo;

class FilePanel : public CTabPage, public CCObject
{
private:
    ListViewer m_fileView;
    TextLabel m_textTitle;
    std::stack<int> m_keyIdStack;
    std::map<std::string, int> m_keyMap;

public:
    explicit FilePanel(UINT nIDTemplate, CWnd *parent);
    ~FilePanel() override;
    
private:
    void InitStack(const size_t preCount);
    bool SearchFromFileName(const std::string &fileName, int &destKey);

private:
    void InitCControls();
    void OnInitialUpdate() override;

public:
    void SlotFileListAppend(std::shared_ptr<DownloadFileInfo> &&updateItem);

protected:
    DECLARE_MESSAGE_MAP()

    void DoDataExchange(CDataExchange *pDX) override;
};

#endif

