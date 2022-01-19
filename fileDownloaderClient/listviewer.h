
#ifndef LIST_VIEWER_H__
#define LIST_VIEWER_H__

#include <vector>
#include <memory>
#include <string>
#include <list>
#include <tuple>
#include <map>

class ListElement;
class ListHeaderPanel;

class ListViewer : public CListCtrl
{
    DECLARE_DYNAMIC(ListViewer)

public:
    class ListColumn;

private:
    using list_element_ty = std::shared_ptr<ListElement>;
    using list_cont_ty = std::vector<list_element_ty>;
    using list_cont_iterator = list_cont_ty::const_iterator;

    list_cont_ty m_listdata;
    std::map<std::string, list_cont_iterator> m_iterMap;
    std::unique_ptr<ListHeaderPanel> m_headerPanel;

public:
    explicit ListViewer();
    ~ListViewer() override;

private:
    void DrawStuff(CDC &cdc);
private:
    bool GetListData(const std::string &uniqKey, list_element_ty &dest);
    bool UpdateChangedCell(list_element_ty &updateData);

public:
    bool Erase(list_element_ty delData);
    bool Append(list_element_ty addData, bool update = true);
    bool AppendWithList(const std::list<list_element_ty> &addDataList);
    void AttachListColumn(const ListColumn &columnData);

private:
    void EnableHighlighting(HWND hWnd, int row, bool bHighlight);
    bool IsRowSelected(HWND hWnd, int row);
    bool IsRowHighlighted(HWND hWnd, int row);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult);
    afx_msg void OnGetDisplayInfoList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnPaint();
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

