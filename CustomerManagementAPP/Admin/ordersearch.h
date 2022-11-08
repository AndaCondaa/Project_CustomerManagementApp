/*
 *  Program Name    :  Admin
 *  File Name       :  ordersearch.h
 *  Description     :  주문 검색 위젯
*/

#ifndef ORDERSEARCH_H
#define ORDERSEARCH_H

#include <QWidget>

namespace Ui {
class OrderSearch;
}

class OrderSearch : public QWidget
{
    Q_OBJECT

public:
    explicit OrderSearch(QWidget *parent = nullptr);
    ~OrderSearch();

signals:
    void search(int, QString);              // Send Search type and keyword to OrderManager

private slots:
    void on_clearButton_clicked();          // Slot connected to Clicked() of ClearButton
    void on_searchButton_clicked();         // Slot connected to Clicked() of SearchButton
    void recvSearchResult(int, int, int, QString, int, int);
                                            // Receive the result of search from OrderManager
private:
    Ui::OrderSearch *ui;
};

#endif // ORDERSEARCH_H
