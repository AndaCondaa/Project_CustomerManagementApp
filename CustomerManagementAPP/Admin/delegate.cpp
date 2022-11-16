#include "delegate.h"


Delegate::Delegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void Delegate::setLogInVector(QVector<int> vector)
{
    logInVector = vector;
}

void Delegate::setChattingVector(QVector<int> vector)
{
    chattingVector = vector;
}

void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem op(option);

    if (logInVector.contains(index.row())) {
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
