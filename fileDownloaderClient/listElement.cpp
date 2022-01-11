
#include "pch.h"
#include "listElement.h"

ListElement::ListElement()
{ }

ListElement::~ListElement()
{ }

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