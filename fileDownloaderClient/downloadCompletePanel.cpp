
#include "pch.h"
#include "downloadCompletePanel.h"
#include "completedFileInfo.h"
#include "iniFileMan.h"
#include "stringhelper.h"
#include "resource.h"

static constexpr auto complete_ini_filename = "completeLog.txt";
static constexpr auto section_key_complete_info = "CompleteInfo";
static constexpr auto key_complete_count = "CompleteCount";
static constexpr auto section_file_key = "DownloadFile";
static constexpr auto file_key_url_key = "fileUrl";
static constexpr auto file_key_size_key = "fileSize";
static constexpr auto file_key_datetime_key = "dateTime";

using namespace _StringHelper;

DownloadCompletePanel::DownloadCompletePanel(UINT nIDTemplate, CWnd *parent)
    : CTabPage(nIDTemplate, parent)
{
    m_completeIni = std::make_unique<IniFileMan>();
}

DownloadCompletePanel::~DownloadCompletePanel()
{ }

void DownloadCompletePanel::UpdateReportView()
{
    using list_element_ty = std::shared_ptr<ListElement>;
    int count = 0;

    m_completeIni->GetItemValue(section_key_complete_info, key_complete_count, count);
    int cur = 0;
    std::string fileSectionKey;
    list_element_ty completeInfo;
    std::list<list_element_ty> infoList;

    while (cur < count)
    {
        fileSectionKey = section_file_key + std::to_string(cur);
        std::string fileUrl;
        if (!m_completeIni->GetItemValue(fileSectionKey, file_key_url_key, fileUrl))
            break;
        completeInfo = std::make_shared<CompletedFileInfo>();
        completeInfo->SetElement(CompletedFileInfo::PropertyInfo::FileUrl, fileUrl);
        std::string fileSize, datetime;
        
        if (m_completeIni->GetItemValue(fileSectionKey, file_key_size_key, fileSize))
            completeInfo->SetElement(CompletedFileInfo::PropertyInfo::FileSize, fileSize);
        if (m_completeIni->GetItemValue(fileSectionKey, file_key_datetime_key, datetime))
            completeInfo->SetElement(CompletedFileInfo::PropertyInfo::DownloadDateTime, datetime);
        infoList.push_back(completeInfo);
        ++cur;
    }

    m_completeList.AppendWithList(infoList);
}

void DownloadCompletePanel::InitCControls()
{
    m_textTitle.SetWindowTextA("Downloaded Files");
    m_textTitle.SetTextColor(RGB(242, 251, 132));
    m_textDesc.SetWindowTextA("-");
    m_textDesc.SetTextColor(RGB(181, 230, 29));
}

void DownloadCompletePanel::InitialLoadIni()
{
    if (!m_completeIni->ReadIni(complete_ini_filename))
    {
        m_completeIni->SetItemValue(section_key_complete_info, key_complete_count, 0);
        m_completeIni->WriteIni();
    }
    else
    {
        UpdateReportView();
    }
}

void DownloadCompletePanel::UpdateItemCountText()
{
    int completeCount = 0;

    if (m_completeIni->GetItemValue(section_key_complete_info, key_complete_count, completeCount))
        m_textDesc.SetWindowTextA(toArray(stringFormat("found %d items", completeCount)));
}

void DownloadCompletePanel::OnInitialUpdate()
{
    ListViewer::ListColumn listHeader;
    CRect viewArea;

    m_completeList.GetClientRect(&viewArea);

    int width = viewArea.right;
    auto setWidthFx = [&width](int columnWidth)
    {
        width = (width < columnWidth) ? 0 : width - columnWidth;
        return (width == 0) ? width : columnWidth;
    };

    listHeader.Append("FileName", setWidthFx(300));
    listHeader.Append("FileSize", setWidthFx(100));
    listHeader.Append("CreatedDateTime", setWidthFx(150));
    listHeader.Append("", width);

    m_completeList.AttachListColumn(listHeader);

    InitialLoadIni();
}

void DownloadCompletePanel::OnEnterScreen()
{
    std::unique_ptr<DownloadCompletePanel, std::function<void(DownloadCompletePanel*)>> sync(this, [this](DownloadCompletePanel *panel) { panel->UpdateItemCountText(); });

    if (m_completeTempList.empty())
        return;

    int completeCount = 0;

    if (m_completeIni->GetItemValue(section_key_complete_info, key_complete_count, completeCount))
        m_completeIni->SetItemValue(section_key_complete_info, key_complete_count, completeCount + m_completeTempList.size());

    int listsize = m_completeList.GetItemCount();

    for (const auto &completeElem : m_completeTempList)
    {
        std::string sectionKey = section_file_key + std::to_string(listsize++);

        m_completeIni->SetItemValue(sectionKey, file_key_url_key, completeElem->GetElement(CompletedFileInfo::PropertyInfo::FileUrl));
        m_completeIni->SetItemValue(sectionKey, file_key_size_key, completeElem->GetElement(CompletedFileInfo::PropertyInfo::FileSize));
        m_completeIni->SetItemValue(sectionKey, file_key_datetime_key, completeElem->GetElement(CompletedFileInfo::PropertyInfo::DownloadDateTime));
    }

    m_completeList.AppendWithList(m_completeTempList);
    m_completeTempList.clear();
}

void DownloadCompletePanel::BeforeDestroy()
{
    m_completeIni->WriteIni();
}

void DownloadCompletePanel::SlotAppendComplete(std::shared_ptr<CompletedFileInfo> addItem)
{
    m_completeTempList.push_back(addItem);
}

void DownloadCompletePanel::DoDataExchange(CDataExchange *pDX)
{
    CTabPage::DoDataExchange(pDX);
    auto controlExchange = [pDX](int nIDC, CWnd &wnd)
    { DDX_Control(pDX, nIDC, wnd); };

    controlExchange(COMPLETE_TITLE, m_textTitle);
    controlExchange(COMPLETE_LIST, m_completeList);
    controlExchange(COMPLETE_DESC, m_textDesc);
    InitCControls();
}