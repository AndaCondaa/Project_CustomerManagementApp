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

signals:
    void search(int, QString);          // Send Search type and keyword to ProductManager

private slots:
    void on_clearButton_clicked();      // Slot connected to Clicked() of ClearButton
    void on_searchButton_clicked();     // Slot connected to Clicked() of SearchButton
    void recvSearchResult(QString, QString, QString, QString);
                                        // Receive the result of search from ProductManger
private:
    Ui::ProductSearch *ui;
};

#endif // PRODUCTSEARCH_H
