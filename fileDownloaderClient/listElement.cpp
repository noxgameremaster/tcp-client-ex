
#include "pch.h"
#include "listElement.h"

ListElement::ListElement()
{
    m_order = 0;
}

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

bool ListElement::Clone(const ListElement *other)
{
    if (other == nullptr)
        return false;
    if (other == this)
        return false;

    *this = *other;
    /*m_order = other->m_order;
    m_testString = other->m_testString;*/
    return true;
}