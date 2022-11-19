/*
 *  Program Name    :  Admin
 *  File Name       :  chatdelegate.h
 *  Description     :  Delegate for CustomerTableView in Class ChatManager
*/


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
    void setWaitVector(QVector<int>);
    void setChattingVector(QVector<int>);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QVector<int> waitVector;
    QVector<int> chattingVector;
};

#endif // CHATDELEGATE_H
