
#include "pch.h"
#include "listElement.h"
#include <sstream>

ListElement::ListElement()
{ }

ListElement::~ListElement()
{ }

template <class Ty>
void ListElement::SetData(Ty &dest, const std::string &src)
{
    std::stringstream ss(src);

    ss >> dest;
}

std::string ListElement::GetElement(int index)
{
    switch (index)
    {
    case 0: return std::to_string(m_order);
    case 1: return m_testString;
    default: return {};
    }
}

void ListElement::SetElement(int index, const std::string &value)
{
    switch (index)
    {
    case 0:
        SetData(m_order, value);
        break;
    case 1:
        m_testString = value;
        break;
    default:
        return;
    }
}