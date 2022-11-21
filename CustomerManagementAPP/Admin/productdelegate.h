/*
 *  Program Name    :  Admin
 *  File Name       :  chatdelegate.h
 *  Description     :  Delegate for ProductTableView in Class ProductManager
*/

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
    void setOutVector(QVector<int>);            // Set new 'Out of Stock' List
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
                                                // Virtual Function for painting background

private:
    QVector<int> stockOutVector;                // Products' row number List in Model
};

#endif // PRODUCTDELEGATE_H
