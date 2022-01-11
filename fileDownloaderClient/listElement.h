
#ifndef LIST_ELEMENT_H__
#define LIST_ELEMENT_H__

#include <string>

class ListElement
{
private:
    int m_order;
    std::string m_testString;

public:
    explicit ListElement();
    virtual ~ListElement();

private:
    template <class Ty>
    void SetData(Ty &dest, const std::string &src);

    virtual size_t ElementCount()
    {
        return 2;
    }

public:
    virtual std::string GetElement(int index);
    virtual void SetElement(int index, const std::string &value);
};

#endif

