/*
 *  Program Name    :  Admin
 *  File Name       :  orderinput.cpp
 *  Description     :  신규 주문을 입력하는 위젯
 *                      -> OrderManager위젯으로 정보를 전달하여, 새로운 주문객체 생성
*/


#include "orderinput.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include <QDateEdit>
#include <QTime>
#include <QRandomGenerator>
#include <QSqlQuery>
#include <QSqlError>


#include <QApplication>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

OrderInput::OrderInput(QWidget *parent)
    : QWidget{parent}
{
    // Setting GUI
    setFixedSize(290, 340);
    setWindowTitle(tr("ORDER INPUT"));

    orderNum = new QLabel(tr("OrderNumber"), this);
    orderCK = new QLabel(tr("CustomerKey"), this);
    orderPK = new QLabel(tr("Product"), this);
    date = new QLabel(tr("Date"), this);
    quantity = new QLabel(tr("Quantity"), this);
    total = new QLabel(tr("Total"), this);

    orderNum->setGeometry(10,20,110,30);
    orderCK->setGeometry(10,50,110,30);
    orderPK->setGeometry(10,80,110,30);
    date->setGeometry(10,110,110,30);
    quantity->setGeometry(10,140,110,30);
    total->setGeometry(10,170,110,30);

    orderNum->setAlignment(Qt::AlignRight);
    orderCK->setAlignment(Qt::AlignRight);
    orderPK->setAlignment(Qt::AlignRight);
    date->setAlignment(Qt::AlignRight);
    quantity->setAlignment(Qt::AlignRight);
    total->setAlignment(Qt::AlignRight);

    orderNumLine = new QLineEdit(tr("Randomly created"), this);
    orderCkBox = new QComboBox(this);
    orderPkBox = new QComboBox(this);
    dateEdit = new QDateEdit(this);
    quantityLine = new QLineEdit(this);
    totalLine = new QLineEdit(this);

    orderNumLine->setGeometry(140,20,130,20);
    orderCkBox->setGeometry(140,50,130,20);
    orderPkBox->setGeometry(140,80,130,20);
    dateEdit->setGeometry(140,110,130,20);
    quantityLine->setGeometry(140,140,130,20);
    totalLine->setGeometry(140,170,130,20);

    dateEdit->setDate(QDate::currentDate());
    dateEdit->setMaximumDate(QDate::currentDate());

    orderNumLine->setReadOnly(true);
    totalLine->setReadOnly(true);

    orderNumLine->setAlignment(Qt::AlignRight);
    dateEdit->setAlignment(Qt::AlignRight);
    quantityLine->setAlignment(Qt::AlignRight);
    totalLine->setAlignment(Qt::AlignRight);

    quantityLine->setInputMask("000");

    clearButton = new QPushButton("&CLEAR", this);
    inputButton = new QPushButton("&INPUT", this);

    clearButton->setGeometry(20,205,115,115);
    inputButton->setGeometry(155,205,115,115);

    // Connecting signals and slots
    connect(clearButton, SIGNAL(clicked()), SLOT(clear()));
    connect(inputButton, SIGNAL(clicked()), SLOT(input()));
    connect(orderPkBox, SIGNAL(currentIndexChanged(int)), SLOT(getInfo(int)));
    connect(quantityLine, SIGNAL(textChanged(QString)), SLOT(calTotal(QString)));
}

// Slot connected to Clicked() of ClearButton
void OrderInput::clear()
{
    orderCkBox->setCurrentIndex(0);
    orderPkBox->setCurrentIndex(0);
    dateEdit->setDate(QDate::currentDate());
    quantityLine->clear();
    totalLine->clear();
}

void OrderInput::input()
{
    QString orderNumber = makeOrderNumber();
    int ck = orderCkBox->currentText().toInt();
    int pk = orderPkBox->currentText().toInt();
    QString date = dateEdit->text();
    int quantity = quantityLine->text().toInt();
    int total = totalLine->text().toInt();

    if (tmp_stock < quantity) {
        QMessageBox overStock;
        overStock.setIcon(QMessageBox::Warning);
        overStock.setText(tr("Quantity is more than the number of stock"));
        overStock.setWindowTitle(tr("OVER STOCK!"));
        overStock.exec();
        return;
    } else if (quantity == 0) {
        QMessageBox zeroQuantity;
        zeroQuantity.setIcon(QMessageBox::Warning);
        zeroQuantity.setText(tr("Quantity should be more than ZERO"));
        zeroQuantity.setWindowTitle(tr("Quantity is ZERO!"));
        zeroQuantity.exec();
        return;
    }

    QSqlDatabase orderDB = QSqlDatabase::database("OrderManager");
    QSqlQuery inputQuery(orderDB);
    inputQuery.prepare("CALL sys.INPUT_ORDER(:orderNum, :ck, :pk, :date, :quantity, :total)");
    inputQuery.bindValue(":orderNum", orderNumber);
    inputQuery.bindValue(":ck", ck);
    inputQuery.bindValue(":pk", pk);
    inputQuery.bindValue(":date", date);
    inputQuery.bindValue(":quantity", quantity);
    inputQuery.bindValue(":total", total);
    bool isInput = inputQuery.exec();

    //재고수량 빼기
    QSqlQuery stock(orderDB);
    stock.prepare("CALL sys.ORDER_STOCK(:pk, :quantity)");
    stock.bindValue(":pk", pk);
    stock.bindValue(":quantity", quantity);
    bool isStock = stock.exec();

    if (isInput && isStock) {
        clear();
        qDebug() << "성공";
        emit inputOrder(ck);
    }
    else {
        if (inputQuery.lastError().text().contains("ORA-00001")) {
            qDebug() << "유니크 에러";
        } else if (inputQuery.lastError().text().contains("ORA-12899"))
            qDebug() << "컬럼 데이터 초과 에러";
    }
}

QString OrderInput::makeOrderNumber()
{
    QString ckIndex = QString::number(orderCkBox->currentText().toInt() / 1000);
    QString pkIndex = QString::number(orderPkBox->currentText().toInt() / 100);

    QString date = dateEdit->text().split("-")[0] + dateEdit->text().split("-")[1] +
                            dateEdit->text().split("-")[2]; //YYYY-MM-DD -> YYYYMMDD
    QString dateIndex = date.right(6);  //YYYYMMDD->YYMMDD
    srand(QTime::currentTime().msecsSinceStartOfDay());     //시드를 주어 매번 랜덤한 값으로 나타냄
    QString tmp = QString::number(rand() % 100);

    QString orderNumber = dateIndex + ckIndex + pkIndex + tmp;

    return orderNumber;
}

void OrderInput::getInfo(int idx)
{
    Q_UNUSED(idx);
    QSqlDatabase orderDB = QSqlDatabase::database("OrderManager");
    QSqlQuery getPrice(orderDB);
    getPrice.prepare("SELECT sys.CHECK_PRICE(:pk) FROM dual");
    getPrice.bindValue(":pk", orderPkBox->currentText().toInt());
    getPrice.exec();
    getPrice.first();
    tmp_price = getPrice.value(0).toInt();
    calTotal(quantityLine->text());

    QSqlQuery getStock(orderDB);
    getStock.prepare("SELECT sys.CHECK_STOCK(:pk) FROM dual");
    getStock.bindValue(":pk", orderPkBox->currentText().toInt());
    getStock.exec();
    getStock.first();
    tmp_stock = getStock.value(0).toInt();
}

void OrderInput::calTotal(QString quantity)
{
    if (quantity.isEmpty()) {
        totalLine->setText("0");
    }
    else {
        totalLine->setText(QString::number(tmp_price * quantity.toInt()));
    }
}

void OrderInput::fillCombo()
{
    orderCkBox->clear();
    orderPkBox->clear();

    QSqlDatabase orderDB = QSqlDatabase::database("OrderManager");
    //ck채우기
    QSqlQuery countCK(orderDB);
    countCK.exec("SELECT sys.count_customer FROM dual");
    countCK.first();
    int countCustomer = countCK.value(0).toInt();
    for (int i = 1; i <= countCustomer; i++) {
        QSqlQuery getCK(orderDB);
        getCK.prepare("SELECT sys.ck_order(:rownum) FROM dual");
        getCK.bindValue(":rownum", i);
        getCK.exec();
        getCK.first();
        orderCkBox->addItem(getCK.value(0).toString());
    }

    //pk채우기
    QSqlQuery countPK(orderDB);
    countPK.exec("SELECT sys.count_product FROM dual");
    countPK.first();
    int countProduct = countPK.value(0).toInt();
    for (int i = 1; i <= countProduct; i++) {
        QSqlQuery getPK(orderDB);
        getPK.prepare("SELECT sys.pk_order(:rownum) FROM dual");
        getPK.bindValue(":rownum", i);
        getPK.exec();
        getPK.first();
        orderPkBox->addItem(getPK.value(0).toString());
    }
}
