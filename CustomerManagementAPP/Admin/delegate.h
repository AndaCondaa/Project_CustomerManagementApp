#ifndef DELEGATE_H
#define DELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QModelIndex>



class Delegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit Delegate(QObject *parent = 0);

    // background color manipulation
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void connect(QModelIndex &index);
    void setLogInVector(QVector<int>);
    void setChattingVector(QVector<int>);

    QVector<int> logInVector;
    QVector<int> chattingVector;
};

#endif // DELEGATE_H
