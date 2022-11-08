/*
 *  Program Name    :  Admin
 *  File Name       :  ordersearch.cpp
 *  Description     :  주문 검색 위젯
*/


#include "ordersearch.h"
#include "ui_ordersearch.h"
#include "order.h"

#include <QMessageBox>

OrderSearch::OrderSearch(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderSearch)
{
    ui->setupUi(this);
    setFixedSize(690, 420);
}

OrderSearch::~OrderSearch()
{
    delete ui;
}

// Slot connected to Clicked() of ClearButton
void OrderSearch::on_clearButton_clicked()
{
    ui->searchEdit->clear();
    ui->treeWidget->clear();
}

// Slot connected to Clicked() of SearchButton
void OrderSearch::on_searchButton_clicked()
{
    ui->treeWidget->clear();
    if (ui->searchEdit->text() == "") {
        QMessageBox nothing;
        nothing.setIcon(QMessageBox::Warning);
        nothing.setText(tr("Please input word for search"));
        nothing.setWindowTitle("NOTHING!!");
        nothing.exec();
    } else {
        // Send Search type and keyword to OrderManager
        emit search(ui->comboBox->currentIndex(),
                    ui->searchEdit->text());
    }
}

// Receive the result of search from OrderManager
void OrderSearch::recvSearchResult
(int orderNum, int orderCk, int orderPk, QString date, int quantity, int total)
{
    Order* order = new Order(orderNum, orderCk, orderPk, date, quantity, total);
    ui->treeWidget->addTopLevelItem(order);
}



