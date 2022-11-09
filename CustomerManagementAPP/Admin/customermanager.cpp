/*
 *  Program Name    :  Admin
 *  File Name       :  customermanager.cpp
 *  Description     :  고객관리 위젯
 *                      -> 등록된 고객 목록 출력
 *                      -> 신규등록 요청 수행
 *                      -> 검색 요청 결과 전송
 *                      -> 정보수정 요청 수행
 *                      -> 주문클래스로 고객정보 전송
*/


#include "customermanager.h"
#include "ui_customermanager.h"
#include "customerinput.h"
#include "customersearch.h"
#include "customeredit.h"

#include <QFile>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

CustomerManager::CustomerManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomerManager)
{
    ui->setupUi(this);
    customerInput = new CustomerInput;
    customerSearch = new CustomerSearch;
    customerEdit = new CustomerEdit;   

    queryModel = new QSqlQueryModel(this);
    queryModel->setQuery("select * from customer order by customer_key");

    ui->customerTableView->setModel(queryModel);
    ui->customerTableView->resizeColumnsToContents();

    connect(this, SIGNAL(sendCurrentCK(int)), customerInput, SLOT(recvCurrentCK(int)));
    connect(customerInput, SIGNAL(inputCustomer()), this, SLOT(update()));
}

CustomerManager::~CustomerManager()
{
    delete ui;
}

// Show the CustomerInput Widget
void CustomerManager::on_inputButton_clicked()
{
    customerInput->show();
    int currentCK = queryModel->index(queryModel->rowCount() - 1, 0).data().toInt();
    emit sendCurrentCK(currentCK);
}

// Show the CustomerSearch Widget
void CustomerManager::on_searchButton_clicked()
{
    customerSearch->show();
}

// Show the CustomerEdit Widget
void CustomerManager::on_editButton_clicked()
{
    customerEdit->show();
}

void CustomerManager::update()
{
    queryModel->setQuery("select * from customer order by customer_key");
    ui->customerTableView->resizeColumnsToContents();
}
