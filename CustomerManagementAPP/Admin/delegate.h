#ifndef DELEGATE_H
#define DELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QSpinBox>

//class Delegate : public QItemDelegate
//{
//public:
//    explicit Delegate(QWidget *parent = 0);

//    void paint(QPainter *painter, const QStyleOptionViewItem &option,
//                   const QModelIndex &index) const;
//};

class Delegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit Delegate(QObject *parent = 0, int row = -1);

    // background color manipulation
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void connect(QModelIndex &index);


    int row;
};

#endif // DELEGATE_H
