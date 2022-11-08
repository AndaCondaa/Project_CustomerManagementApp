/*
 *  Program Name    :  Admin
 *  File Name       :  productsearch.cpp
 *  Description     :  상품 검색 위젯
*/


#include "productsearch.h"
#include "ui_productsearch.h"
#include "product.h"

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

// Slot connected to Clicked() of SearchButton
void ProductSearch::on_searchButton_clicked()
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
void ProductSearch::recvSearchResult(QString pk, QString type, QString name, QString price)
{
    Product* product = new Product(pk.toInt(), type, name, price.toInt());
    ui->treeWidget->addTopLevelItem(product);
}


