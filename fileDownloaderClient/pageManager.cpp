
#include "pch.h"
#include "PageManager.h"
#include "cTabPage.h"
#include <algorithm>

PageManager::PageManager(const CWnd &targetWnd, CWnd *parent)
{
    m_parentWnd = parent;
    ::ZeroMemory(&m_context, sizeof(m_context));
    targetWnd.GetWindowRect(&m_screenArea);
    parent->ScreenToClient(&m_screenArea);
}

//@brief. 소멸자
PageManager::~PageManager()
{ }

//@brief. 페이지 생성
bool PageManager::MakePage(const std::string &pageKey, std::unique_ptr<CWnd> wndObject)
{
    std::map<std::string, std::unique_ptr<CWnd>>::iterator objectIterator = m_objectMap.find(pageKey);

    if (objectIterator != m_objectMap.end())
    {
        //@brief. already exist!
        return false;
    }
    CTabPage *tabPage = dynamic_cast<CTabPage *>(wndObject.get());

    if (tabPage->CreatePage(m_screenArea, &m_context))
    {
        tabPage->OnInitialUpdate();
        tabPage->SetPageName(pageKey);
        m_objectMap.emplace(pageKey, std::move(wndObject));
        return true;
    }
    return false;
}

//@brief. 특정 페이지를 보여줍니다
bool PageManager::ShowPage(const std::string &pageKey)
{
    std::map<std::string, std::unique_ptr<CWnd>>::iterator objectIterator = m_objectMap.find(pageKey);

    while (objectIterator != m_objectMap.end())
    {
        if (!m_currentScreenName.compare(pageKey))
            return true;

        CTabPage *currentPage = dynamic_cast<CTabPage *>(objectIterator->second.get());
        if (currentPage == nullptr)
            break;

        currentPage->Show();
        while (m_currentScreenName.length())
        {
            CTabPage *previousPage = dynamic_cast<CTabPage *>(m_objectMap[m_currentScreenName].get());
            if (previousPage == nullptr)
                break;
            previousPage->Show(false);
            break;
        }
        m_currentScreenName = pageKey;
        return true;
    }
    return false;
}

//@brief. 생성된 탭을 모두 지웁니다	//아직까지 안쓰이는 메서드입니다
void PageManager::DestroyAll()
{
    for (auto &page : m_objectMap)
    {
        page.second->DestroyWindow();
        page.second.reset();
    }
    m_objectMap.clear();
}

void PageManager::SetSelf(std::shared_ptr<CCObject> self)
{
    m_self = self;
}

CWnd *PageManager::FindScreen(const std::string &screenName)
{
    auto screenFind = m_objectMap.find(screenName);

    if (screenFind != m_objectMap.end())
    {
        return screenFind->second.get();
    }
    return nullptr;
}




