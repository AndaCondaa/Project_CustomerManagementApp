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

OrderManager::OrderManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderManager)
{
    ui->setupUi(this);
    orderInput = new OrderInput;
    orderQueryModel = new QSqlQueryModel(ui->orderTableView);
    priceModel = new QStandardItemModel(0,2);

    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setMaximumDate(QDate::currentDate());

    updateTable();

    connect(orderInput, SIGNAL(inputOrder()), this, SLOT(update()));    //주문 인풋 후 메인테이블 업데이트하라고 알려주기
    connect(this, SIGNAL(sendCkToInput(QVector<int>)), orderInput, SLOT(recvCkList(QVector<int>))); //ck리스트 전송 연결
    connect(this, SIGNAL(sendPkToInput(QVector<int>)), orderInput, SLOT(recvPkList(QVector<int>)));
    connect(orderInput, SIGNAL(sendPkToManager(QString)), this, SLOT(recvPkFromInput(QString)));
    connect(this, SIGNAL(sendResultPrice(QStringList)),
            orderInput,SLOT(recvPriceModel(QStringList)));
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

void OrderManager::recvPkFromInput(QString pk)
{
    emit checkPrice(pk);
}

void OrderManager::recvResultPrice(QStringList list)
{
    priceModel->clear();
    QList<QStandardItem *> result;
    for (int i = 0; i < 2; ++i) {
        result.append(new QStandardItem(list.at(i)));
    }
    priceModel->appendRow(result);

    emit sendResultPrice(list);
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

    QString searchFlag = ui->searchComboBox->currentText();
    QString searchWord = ui->searchLine->text();

    orderQueryModel->setQuery
            (QString("SELECT * FROM ORDER_TABLE WHERE %1 LIKE '%%2%'")
                                        .arg(searchFlag, searchWord));
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
    ui->pkBox->setCurrentIndex(0);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->quantityLine->clear();
    ui->totalLine->clear();
}

void OrderManager::on_editButton_clicked()
{
    QSqlQuery editQuery;
    editQuery.prepare("CALL EDIT_ORDER (:orderNum, :ck, :pk, :date, :quantity, :total)");
    editQuery.bindValue(":orderNum", ui->orderNumLine->text());
    editQuery.bindValue(":ck", ui->ckLine->text());
    editQuery.bindValue(":pk", ui->pkBox->currentText());
    editQuery.bindValue(":date", ui->dateEdit->text());
    editQuery.bindValue(":quantity", ui->quantityLine->text());
    editQuery.bindValue(":total", ui->totalLine->text());
    bool isExec = editQuery.exec();

    if (isExec) {
        ui->orderNumLine->clear();
        ui->ckLine->clear();
        ui->pkBox->setCurrentIndex(0);
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
    ui->pkBox->setCurrentIndex(ui->pkBox->findText(pk));
    ui->dateEdit->setDate(QDate::fromString(date, "YYYY-MM-DD"));
    ui->quantityLine->setText(quantity);
    ui->totalLine->setText(total);
}

void OrderManager::on_pkBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    emit checkPrice(ui->pkBox->currentText());
    on_quantityLine_textChanged(ui->quantityLine->text());
}


void OrderManager::on_quantityLine_textChanged(const QString &quantity)
{
    if (ui->quantityLine->text().isEmpty()) {
        ui->totalLine->setText("0");
    }
    else {
        int price = priceModel->item(0, 1)->text().toInt();
        ui->totalLine->setText(QString::number(price * quantity.toInt()));
    }
}

