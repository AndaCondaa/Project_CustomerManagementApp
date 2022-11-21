#include "productdelegate.h"

ProductDelegate::ProductDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{
}

// Set new 'Out of Stock' List
void ProductDelegate::setOutVector(QVector<int> vector)
{
    stockOutVector = vector;
}

// Virtual Function for painting background
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
