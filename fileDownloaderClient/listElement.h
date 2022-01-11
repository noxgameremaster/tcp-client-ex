
#ifndef LIST_ELEMENT_H__
#define LIST_ELEMENT_H__

#include <sstream>
#include <string>

class ListElement
{
private:
    int m_order;
    std::string m_testString;

public:
    explicit ListElement();
    virtual ~ListElement();

protected:
    template <class Ty>
    void SetData(Ty &dest, const std::string &src)
    {
        std::stringstream ss(src);

        ss >> dest;
    }

private:
    virtual size_t ElementCount()
    {
        return 2;
    }

public:
    virtual std::string GetElement(int index);
    virtual void SetElement(int index, const std::string &value);
};

#endif

