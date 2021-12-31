
#ifndef PAGE_MANAGER_H__
#define PAGE_MANAGER_H__

#include "ccobject.h"
#include <string>
#include <map>

class PageManager : public CCObject
{
private:
    CCreateContext m_context;
    CRect m_screenArea;
    CWnd *m_parentWnd;
    std::map<std::string, std::unique_ptr<CWnd>> m_objectMap;
    std::string m_currentScreenName;
    std::weak_ptr<CCObject> m_self;

public:
    explicit PageManager(const CWnd &target, CWnd *parent);
    PageManager(const PageManager &) = delete;
    PageManager(PageManager &&) = delete;
    virtual ~PageManager() override;
    PageManager &operator=(const PageManager &) = delete;

    bool MakePage(const std::string &pageKey, std::unique_ptr<CWnd> wndObject);
    bool ShowPage(const std::string &pageKey);

    //friend struct ConcretePageManager;

private:
    void DestroyAll();
    void SetSelf(std::shared_ptr<CCObject> self);

    CWnd *FindScreen(const std::string &screenName);
};

#endif


