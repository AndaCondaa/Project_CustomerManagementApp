#include "delegate.h"


Delegate::Delegate(QObject *parent, int row)
    : QStyledItemDelegate(parent)
{
    this->row = row;
}

void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem op(option);

    if (index.row() == this->row) {
        QColor background = QColor(135, 206, 255);
        painter->fillRect(op.rect, background);
    }

    // Paint text
    QStyledItemDelegate::paint(painter, option, index);
}
