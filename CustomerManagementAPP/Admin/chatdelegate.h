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
    void setWaitVector(QVector<int>);               // Set new Waitting List
    void setChattingVector(QVector<int>);           // Set new Chatting List
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
                                                    // Virtual Function for painting background
private:
    QVector<int> waitVector;                // Waitting Customer's row number List in Model
    QVector<int> chattingVector;            // Chatting Customer's row number List in Model
};

#endif // CHATDELEGATE_H
