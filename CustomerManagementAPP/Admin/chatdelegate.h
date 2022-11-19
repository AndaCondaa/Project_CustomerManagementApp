#ifndef CHATDELEGATE_H
#define CHATDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QModelIndex>

class ChatDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ChatDelegate(QObject *parent = 0);

    // background color manipulation
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setWaitVector(QVector<int>);
    void setChattingVector(QVector<int>);

    QVector<int> waitVector;
    QVector<int> chattingVector;
};

#endif // CHATDELEGATE_H
