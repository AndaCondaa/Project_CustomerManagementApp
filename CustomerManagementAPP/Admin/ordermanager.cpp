/*
 *  Program Name    :  Admin
 *  File Name       :  ordermanager.cpp
 *  Description     :  주문관리 위젯
 *                      -> 등록된 주문 목록 출력
 *                      -> 신규주문 요청 수행
 *                      -> 검색 요청 결과 전송
 *                      -> 정보수정 요청 수행
*/


#include "ordermanager.h"
#include "ui_ordermanager.h"
#include "orderinput.h"

#include <QFile>
#include <QMessageBox>
#include <QSqlQueryModel>

OrderManager::OrderManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderManager)
{
    ui->setupUi(this);
    orderInput = new OrderInput;
    orderQueryModel = new QSqlQueryModel(ui->orderTableView);

    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setMaximumDate(QDate::currentDate());

    updateTable();

    connect(orderInput, SIGNAL(inputOrder()), this, SLOT(update()));    //주문 인풋 후 메인테이블 업데이트하라고 알려주기
    connect(this, SIGNAL(sendCkToInput(QVector<int>)), orderInput, SLOT(recvCkList(QVector<int>))); //ck리스트 전송 연결
    connect(this, SIGNAL(sendPkToInput(QVector<int>)), orderInput, SLOT(recvPkList(QVector<int>)));
}

OrderManager::~OrderManager()
{
    delete ui;
}

// Calculate Total price
int OrderManager::makeTotal(int price, int quantity)
{
    int total = price * quantity;
    return total;
}

void OrderManager::on_inputButton_clicked()
{
    orderInput->show();
}

void OrderManager::updateTable()
{
    orderQueryModel->setQuery("SELECT * FROM ORDER_TABLE ORDER BY ORDER_NUMBER");
    orderQueryModel->setHeaderData(0, Qt::Horizontal, tr("OrderNumber"));
    orderQueryModel->setHeaderData(1, Qt::Horizontal, tr("CustomerKey"));
    orderQueryModel->setHeaderData(2, Qt::Horizontal, tr("ProductKey"));
    orderQueryModel->setHeaderData(3, Qt::Horizontal, tr("date"));
    orderQueryModel->setHeaderData(4, Qt::Horizontal, tr("Quantity"));
    orderQueryModel->setHeaderData(5, Qt::Horizontal, tr("Total"));

    ui->orderTableView->setModel(orderQueryModel);
    ui->orderTableView->horizontalHeader()->setStretchLastSection(true);
    ui->orderTableView->horizontalHeader()->setStyleSheet(
                "QHeaderView { font-size: 10pt; color: blue; }");
    ui->orderTableView->resizeColumnsToContents();
}

void OrderManager::recvCustomerKey(QVector<int> ckVector)
{
    emit sendCkToInput(ckVector);

    ui->ckBox->clear();
    foreach (auto ck, ckVector) {
        ui->ckBox->addItem(QString::number(ck));
    }
}

void OrderManager::recvProductKey(QVector<int> pkVector)
{
    emit sendPkToInput(pkVector);

    ui->pkBox->clear();
    foreach (auto pk, pkVector) {
        ui->pkBox->addItem(QString::number(pk));
    }
}

void OrderManager::update()
{
    updateTable();
}

