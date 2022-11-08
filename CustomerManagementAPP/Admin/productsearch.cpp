/*
 *  Program Name    :  Admin
 *  File Name       :  productsearch.cpp
 *  Description     :  상품 검색 위젯
*/


#include "productsearch.h"
#include "ui_productsearch.h"

#include <QMessageBox>

ProductSearch::ProductSearch(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProductSearch)
{
    ui->setupUi(this);
    setFixedSize(690, 420);
}

ProductSearch::~ProductSearch()
{
    delete ui;
}

// Slot connected to Clicked() of ClearButton
void ProductSearch::on_clearButton_clicked()
{
    ui->searchEdit->clear();
    ui->treeWidget->clear();
}
