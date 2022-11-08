/*
 *  Program Name    :  Admin
 *  File Name       :  customersearch.cpp
 *  Description     :  고객 검색 위젯
*/


#include "customersearch.h"
#include "ui_customersearch.h"
#include "customer.h"

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

// Slot connected to Clicked() of SearchButton
void CustomerSearch::on_searchButton_clicked()
{
    ui->treeWidget->clear();
    if (ui->searchEdit->text() == "") {
        QMessageBox nothing;
        nothing.setIcon(QMessageBox::Warning);
        nothing.setText("Please input word for search");
        nothing.setWindowTitle("NOTHING!!");
        nothing.exec();
    } else {
        emit search(ui->comboBox->currentIndex(),
                    ui->searchEdit->text());
    }
}

// Receive the result of search from CustomerManager
void CustomerSearch::recvSearchResult(QString ck, QString clinic, QString license,
                                      QString dentist, QString number)
{
    Customer* customer = new Customer(ck.toInt(), clinic, license,
                                      dentist, number);
    ui->treeWidget->addTopLevelItem(customer);
}



