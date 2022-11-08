/*
 *  Program Name    :  Admin
 *  File Name       :  customersearch.cpp
 *  Description     :  고객 검색 위젯
*/


#include "customersearch.h"
#include "ui_customersearch.h"

#include <QMessageBox>

CustomerSearch::CustomerSearch(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomerSearch)
{
    ui->setupUi(this);
    setFixedSize(690, 420);
}

CustomerSearch::~CustomerSearch()
{
    delete ui;
}

// Slot connected to Clicked() of ClearButton
void CustomerSearch::on_clearButton_clicked()
{
    ui->searchEdit->clear();
    ui->treeWidget->clear();
}



