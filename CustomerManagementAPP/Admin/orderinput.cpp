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

OrderInput::OrderInput(QWidget *parent)
    : QWidget{parent}
{
    // Setting GUI
    setFixedSize(290, 340);

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
    totalLine = new QLineEdit(tr("Automatic calculation"),this);

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

    clearButton = new QPushButton("&CLEAR", this);
    inputButton = new QPushButton("&INPUT", this);

    clearButton->setGeometry(20,205,115,115);
    inputButton->setGeometry(155,205,115,115);

    // Connecting signals and slots
    connect(clearButton, SIGNAL(clicked()), SLOT(clear()));
    connect(inputButton, SIGNAL(clicked()), SLOT(inputEmit()));
}

// Receive the CustomerKey from OrderManager and add ComboBox
void OrderInput::recvCustomerKey(QString ck, bool check)
{
    int index = orderCkBox->count();
    if (check) {
        orderCkBox->insertItem(index, ck);
    } else if (!check){
        index = orderCkBox->findText(ck);
        orderCkBox->removeItem(index);
    }
}

// Receive the ProductKey from OrderManager and add ComboBox
void OrderInput::recvProductKey(QString pk, bool check)
{
    int index = orderPkBox->count();
    if (check) {
        orderPkBox->insertItem(index, pk);
    } else if (!check){
        index = orderPkBox->findText(pk);
        orderPkBox->removeItem(index);
    }
}

// Slot connected to Clicked() of ClearButton
void OrderInput::clear()
{
    orderCkBox->setCurrentIndex(0);
    orderPkBox->setCurrentIndex(0);
    dateEdit->setDate(QDate::currentDate());
    quantityLine->clear();
}

// Slot connected to Clicked() of InputButton
void OrderInput::inputEmit()
{
    // Send inputted result to OrderManager for checking
    emit input(orderCkBox->currentText(), orderPkBox->currentText(),
               dateEdit->date().toString(), quantityLine->text());
    clear();
}
