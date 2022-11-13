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
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QStandardItemModel>


#include <QApplication>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>


OrderManager::OrderManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderManager)
{
    ui->setupUi(this);

    QSqlDatabase orderDB = QSqlDatabase::addDatabase("QODBC", "OrderManager");
    orderDB.setDatabaseName("Oracle11gx64");
    orderDB.setUserName("ORDER_MANAGER");
    orderDB.setPassword("om");
    if (!orderDB.open()) {
        qDebug() << orderDB.lastError().text();
    } else {
        qDebug("Order DB connect success");
    }

    orderInput = new OrderInput;
    orderQueryModel = new QSqlQueryModel(ui->orderTableView);

    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setMaximumDate(QDate::currentDate());

    updateTable();

    connect(orderInput, SIGNAL(inputOrder()), this, SLOT(update()));    //주문 인풋 후 메인테이블 업데이트하라고 알려주기
}

OrderManager::~OrderManager()
{
    delete ui;
    QSqlDatabase orderDB = QSqlDatabase::database("OrderManager");
    if(orderDB.isOpen()) {
        delete orderQueryModel;
        orderDB.commit();
        orderDB.close();
    }
    orderInput->close();
    orderInput->deleteLater();
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
    QSqlDatabase orderDB = QSqlDatabase::database("OrderManager");
    orderQueryModel->setQuery("SELECT * FROM sys.ORDER_TABLE ORDER BY ORDER_NUMBER", orderDB);
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
}

void OrderManager::update()
{
    updateTable();
}

void OrderManager::on_totalButton_clicked()
{
    updateTable();
    ui->searchComboBox->setCurrentIndex(0);
    ui->searchLine->clear();
}

void OrderManager::on_searchButton_clicked()
{
    if (!ui->searchLine->text().length()) {
        QMessageBox nothing;
        nothing.setIcon(QMessageBox::Warning);
        nothing.setText(tr("Please input word for search"));
        nothing.setWindowTitle(tr("NOTHING INPUT for Search"));
        nothing.exec();
        return;
    }

    QString searchColumn = ui->searchComboBox->currentText();
    QString searchWord = ui->searchLine->text();

    QSqlDatabase orderDB = QSqlDatabase::database("OrderManager");
    orderQueryModel->setQuery
            (QString("SELECT * FROM sys.ORDER_TABLE WHERE %1 LIKE '%%2%'")
                                        .arg(searchColumn, searchWord), orderDB);
}


void OrderManager::on_searchComboBox_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        ui->searchLine->clear();
        ui->searchLine->setInputMask("0000000000000");
        ui->searchLine->setPlaceholderText(tr("Input OrderNumber"));
        break;
    case 1:
        ui->searchLine->clear();
        ui->searchLine->setInputMask("000000");
        ui->searchLine->setPlaceholderText(tr("Input CustomerKey"));
        break;
    case 2:
        ui->searchLine->clear();
        ui->searchLine->setInputMask("0000");
        ui->searchLine->setPlaceholderText(tr("Input ProductKey"));
        break;
    case 3:
        ui->searchLine->clear();
        ui->searchLine->setInputMask("0000-00-00");
        ui->searchLine->setPlaceholderText(tr("Input Date"));
        break;
    }
}

void OrderManager::on_clearButton_clicked()
{
    ui->orderNumLine->clear();
    ui->ckLine->clear();
    ui->pkLine->clear();
    ui->dateEdit->setDate(QDate::currentDate());
    ui->quantityLine->clear();
    ui->totalLine->clear();
}

void OrderManager::on_editButton_clicked()
{
    QSqlDatabase orderDB = QSqlDatabase::database("OrderManager");
    QSqlQuery editQuery(orderDB);
    editQuery.prepare("CALL sys.EDIT_ORDER (:orderNum, :ck, :pk, :date, :quantity, :total)");
    editQuery.bindValue(":orderNum", ui->orderNumLine->text());
    editQuery.bindValue(":ck", ui->ckLine->text());
    editQuery.bindValue(":pk", ui->pkLine->text());
    editQuery.bindValue(":date", ui->dateEdit->text());
    editQuery.bindValue(":quantity", ui->quantityLine->text());
    editQuery.bindValue(":total", ui->totalLine->text());
    bool isExec = editQuery.exec();

    if (isExec) {
        ui->orderNumLine->clear();
        ui->ckLine->clear();
        ui->pkLine->clear();
        ui->dateEdit->setDate(QDate::currentDate());
        ui->quantityLine->clear();
        ui->totalLine->clear();
        updateTable();
        qDebug() << tr("EDIT succeed!");
    } else
        qDebug() << tr("EDTI fail!");
}

void OrderManager::on_orderTableView_clicked(const QModelIndex &index)
{
    QString orderNum = orderQueryModel->data(index.siblingAtColumn(0)).toString();
    QString ck = orderQueryModel->data(index.siblingAtColumn(1)).toString();
    QString pk = orderQueryModel->data(index.siblingAtColumn(2)).toString();
    QString date = orderQueryModel->data(index.siblingAtColumn(3)).toString();
    QString quantity = orderQueryModel->data(index.siblingAtColumn(4)).toString();
    QString total = orderQueryModel->data(index.siblingAtColumn(5)).toString();

    ui->orderNumLine->setText(orderNum);
    ui->ckLine->setText(ck);
    ui->pkLine->setText(pk);
    ui->dateEdit->setDate(QDate::fromString(date, "YYYY-MM-DD"));
    ui->quantityLine->setText(quantity);
    ui->totalLine->setText(total);
}

void OrderManager::on_pkLine_textChanged(const QString &pk)
{
    QSqlDatabase orderDB = QSqlDatabase::database("OrderManager");
    QSqlQuery getPrice(orderDB);
    getPrice.prepare("SELECT sys.check_price(:pk) FROM dual");
    getPrice.bindValue(":pk", pk);
    getPrice.exec();
    getPrice.first();
    tmp_price = getPrice.value(0).toInt();
}

void OrderManager::on_quantityLine_textChanged(const QString &quantity)
{
    if (quantity.isEmpty()) {
        ui->totalLine->clear();
    }
    else {
        ui->totalLine->setText(QString::number(tmp_price * quantity.toInt()));
    }
}

