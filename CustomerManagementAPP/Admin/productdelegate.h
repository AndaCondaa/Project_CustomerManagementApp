#ifndef PRODUCTDELEGATE_H
#define PRODUCTDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QModelIndex>

class ProductDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ProductDelegate(QObject *parent = 0);

    // background color manipulation
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setOutVector(QVector<int>);

    QVector<int> stockOutVector;
};

#endif // PRODUCTDELEGATE_H
