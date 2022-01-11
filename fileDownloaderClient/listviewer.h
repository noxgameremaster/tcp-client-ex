
#ifndef LIST_VIEWER_H__
#define LIST_VIEWER_H__

#include <vector>
#include <memory>
#include <string>
#include <list>

class ListElement;

class ListViewer : public CListCtrl
{
    DECLARE_DYNAMIC(ListViewer)

public:
    class ListColumn;

private:
    using list_element_ty = std::unique_ptr<ListElement>;
    std::vector<list_element_ty> m_listdata;

public:
    explicit ListViewer();
    ~ListViewer() override;

public:
    void Append(list_element_ty addData);
    void AttachListColumn(const ListColumn &columnData);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnGetDisplayInfoList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
    void PreSubclassWindow() override;
};

class ListViewer::ListColumn
{
    friend ListViewer;
private:
    std::list<std::tuple<std::string, int>> m_columns;

public:
    ListColumn()
    { }

    ~ListColumn()
    { }

    void Append(const std::string &columnName, int width)
    {
        m_columns.emplace_back(columnName, width);
    }
};

#endif

