#include "chatdelegate.h"

ChatDelegate::ChatDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

// Set new Waitting List
void ChatDelegate::setWaitVector(QVector<int> vector)
{
    waitVector = vector;
}

// Set new Chatting List
void ChatDelegate::setChattingVector(QVector<int> vector)
{
    chattingVector = vector;
}

// Virtual Function for painting background
void ChatDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem op(option);

    if (waitVector.contains(index.row())) {
        QColor background = QColor(250, 244, 192);
        painter->fillRect(op.rect, background);
    } else if (chattingVector.contains(index.row())) {
        QColor background = QColor(171, 242, 0);
        painter->fillRect(op.rect, background);
    } else {
        QColor background = QColor(234, 234, 234);
        painter->fillRect(op.rect, background);
    }

    // Paint text
    QStyledItemDelegate::paint(painter, option, index);
}
