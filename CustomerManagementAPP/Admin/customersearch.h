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

private slots:
    void on_clearButton_clicked();              // Slot connected to Clicked() of ClearButton

private:
    Ui::CustomerSearch *ui;
};

#endif // CUSTOMERSEARCH_H
