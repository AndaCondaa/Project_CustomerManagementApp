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
    void setWaitVector(QVector<int>);               // Set new Waitting List
    void setChattingVector(QVector<int>);           // Set new Chatting List
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
                                                    // Virtual Function for painting background

    QVector<int> waitVector;                // Waitting Customer's row number List in Model
    QVector<int> chattingVector;            // Chatting Customer's row number List in Model
};

#endif // DELEGATE_H
