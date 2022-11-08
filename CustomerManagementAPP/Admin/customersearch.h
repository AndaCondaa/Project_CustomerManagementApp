/*
 *  Program Name    :  Admin
 *  File Name       :  customersearch.h
 *  Description     :  고객 검색 위젯
*/

#ifndef CUSTOMERSEARCH_H
#define CUSTOMERSEARCH_H

#include <QWidget>

class Customer;

namespace Ui {
class CustomerSearch;
}

class CustomerSearch : public QWidget
{
    Q_OBJECT
public:
    explicit CustomerSearch(QWidget *parent = nullptr);
    ~CustomerSearch();

signals:
    void search(int, QString);                  // Send Search type and keyword to CustomerManager

private slots:
    void on_clearButton_clicked();              // Slot connected to Clicked() of ClearButton
    void on_searchButton_clicked();             // Slot connected to Clicked() of SearchButton
    void recvSearchResult(QString, QString, QString, QString, QString);
                                                // Receive the result of search from CustomerManager
private:
    Ui::CustomerSearch *ui;
};

#endif // CUSTOMERSEARCH_H
