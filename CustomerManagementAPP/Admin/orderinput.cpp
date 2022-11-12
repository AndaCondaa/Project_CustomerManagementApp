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
#include <QDateEdit>
#include <QTime>
#include <QRandomGenerator>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardItemModel>


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
    quantityLine->setAlignment(Qt::AlignRight);
    totalLine->setAlignment(Qt::AlignRight);

    quantityLine->setInputMask("000");

    clearButton = new QPushButton("&CLEAR", this);
    inputButton = new QPushButton("&INPUT", this);

    clearButton->setGeometry(20,205,115,115);
    inputButton->setGeometry(155,205,115,115);


    priceModel = new QStandardItemModel(0,2);

    // Connecting signals and slots
    connect(clearButton, SIGNAL(clicked()), SLOT(clear()));
    connect(inputButton, SIGNAL(clicked()), SLOT(input()));
    connect(orderPkBox, SIGNAL(currentIndexChanged(int)), SLOT(sendPk(int)));
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

void OrderInput::recvCkList(QVector<int> ckVector)
{
    orderCkBox->clear();
    foreach (auto ck, ckVector) {
        orderCkBox->addItem(QString::number(ck));
    }
}

void OrderInput::recvPkList(QVector<int> pkVector)
{
    orderPkBox->clear();
    foreach (auto pk, pkVector) {
        orderPkBox->addItem(QString::number(pk));
    }
}

void OrderInput::input()
{
    QString orderNumber = makeOrderNumber();
    int ck = orderCkBox->currentText().toInt();
    int pk = orderPkBox->currentText().toInt();
    QString date = dateEdit->text();
    int quantity = quantityLine->text().toInt();
    int total = totalLine->text().toInt();


    QSqlQuery inputQuery;
    inputQuery.prepare("CALL INPUT_ORDER(:orderNum, :ck, :pk, :date, :quantity, :total)");
    inputQuery.bindValue(":orderNum", orderNumber);
    inputQuery.bindValue(":ck", ck);
    inputQuery.bindValue(":pk", pk);
    inputQuery.bindValue(":date", date);
    inputQuery.bindValue(":quantity", quantity);
    inputQuery.bindValue(":total", total);
    bool isExec = inputQuery.exec();

    if (isExec) {
        clear();
        qDebug() << "성공";
        emit inputOrder();
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

void OrderInput::sendPk(int idx)
{
    Q_UNUSED(idx);
    QString pk = orderPkBox->currentText();
    emit sendPkToManager(pk);
}

void OrderInput::recvPriceModel(QStringList list)
{
    priceModel->clear();
    QList<QStandardItem *> result;
    for (int i = 0; i < 2; ++i) {
        result.append(new QStandardItem(list.at(i)));
    }
    priceModel->appendRow(result);
    calTotal(list.at(1));
}

void OrderInput::calTotal(QString quantity)
{
    if (quantityLine->text().isEmpty()) {
        totalLine->setText("0");
    }
    else {
        int price = priceModel->item(0, 1)->text().toInt();
        totalLine->setText(QString::number(price * quantity.toInt()));
    }
}
