#include "ListWithWxtremes.hpp"

ListWithExtremes::ListWithExtremes() {}
QList<QPointF>* ListWithExtremes::getList()
{
    return &this->list;
}
qreal ListWithExtremes::max()
{
    if(values.size() == 0) return 0.0;
    return this->values.rbegin()->first;
}
qreal ListWithExtremes::min()
{
    if(values.size() == 0) return 0.0;
    return this->values.begin()->first;
}
void ListWithExtremes::appendLastValue(QPointF new_point)
{
    list.append(new_point);
    if(this->values.find(new_point.y()) == this->values.end())
        this->values[new_point.y()] = 0;

    this->values[new_point.y()]++;
}
void ListWithExtremes::deleteFirstValue()
{
    this->values[list.front().y()]--;

    if(this->values[list.front().y()] == 0)
        this->values.erase(list.front().y());

    list.pop_front();
}
void ListWithExtremes::clear()
{
    this->list.clear();
    this->values.clear();
}
