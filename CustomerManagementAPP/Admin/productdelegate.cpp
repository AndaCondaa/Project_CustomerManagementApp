#include "productdelegate.h"

ProductDelegate::ProductDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{
}

void ProductDelegate::setOutVector(QVector<int> vector)
{
    stockOutVector = vector;
}

void ProductDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem op(option);

    if (stockOutVector.contains(index.row())) {
        QColor background = QColor(255, 167, 167);
        painter->fillRect(op.rect, background);
    }
    // Paint text
    QStyledItemDelegate::paint(painter, option, index);
}
