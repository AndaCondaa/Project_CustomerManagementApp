/*
 *  Program Name    :  Admin
 *  File Name       :  ordersearch.cpp
 *  Description     :  주문 검색 위젯
*/


#include "ordersearch.h"
#include "ui_ordersearch.h"

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
