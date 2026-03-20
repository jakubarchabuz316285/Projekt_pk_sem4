#ifndef LISTWITHWXTREMES_H
#define LISTWITHWXTREMES_H
#include <qlist.h>
#include <qpoint.h>
#include <map>


/**
 * @brief Klasa przechowująca dane do wyświetlenie na wykresach. Pozwala na odszukanie i zwrócenie najmniejsze i największej wartości w stałycm czasie
 *
 */
class ListWithExtremes
{
    QList<QPointF> list;
    std::map<qreal, uint16_t> values;
public:

    ListWithExtremes();
    QList<QPointF>* getList();
    qreal max();
    qreal min();
    void appendLastValue(QPointF new_point);
    void deleteFirstValue();
    void clear();
};

#endif // LISTWITHWXTREMES_H
