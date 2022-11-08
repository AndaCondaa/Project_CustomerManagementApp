/*
 *  Program Name    :  Admin
 *  File Name       :  productsearch.h
 *  Description     :  상품 검색 위젯
*/

#ifndef PRODUCTSEARCH_H
#define PRODUCTSEARCH_H

#include <QWidget>

namespace Ui {
class ProductSearch;
}

class ProductSearch : public QWidget
{
    Q_OBJECT

public:
    explicit ProductSearch(QWidget *parent = nullptr);
    ~ProductSearch();

private slots:
    void on_clearButton_clicked();      // Slot connected to Clicked() of ClearButton

private:
    Ui::ProductSearch *ui;
};

#endif // PRODUCTSEARCH_H
