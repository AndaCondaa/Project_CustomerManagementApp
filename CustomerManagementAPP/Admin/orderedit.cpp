/*
 *  Program Name    :  Admin
 *  File Name       :  orderedit.cpp
 *  Description     :  주문 정보를 수정하는 위젯
*/


#include "orderedit.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QDateEdit>
#include <QMessageBox>

OrderEdit::OrderEdit(QWidget *parent)
    : QWidget{parent}
{
    // Set GUI
    setFixedSize(290, 450);

    searchOrderNumber = new QLabel("OrderNumber", this);
    searchLine = new QLineEdit(this);
    searchButton = new QPushButton(tr("SEARCH"), this);

    searchOrderNumber->setGeometry(40,20,230,20);
    searchLine->setGeometry(40,50,230,20);
    searchButton->setGeometry(40,80,230,30);

    searchOrderNumber->setAlignment(Qt::AlignCenter);
    searchLine->setAlignment(Qt::AlignRight);

    orderNumber = new QLabel(tr("OrderNumber"), this);
    orderCustomer = new QLabel(tr("CustomerKey"), this);
    orderProduct = new QLabel(tr("ProductKey"), this);
    date = new QLabel(tr("Date"), this);
    quantity = new QLabel(tr("Quantity"), this);

    orderNumber->setGeometry(10,130,110,30);
    orderCustomer->setGeometry(10,160,110,30);
    orderProduct->setGeometry(10,190,110,30);
    date->setGeometry(10,220,110,30);
    quantity->setGeometry(10,250,110,30);

    orderNumber->setAlignment(Qt::AlignRight);
    orderCustomer->setAlignment(Qt::AlignRight);
    orderProduct->setAlignment(Qt::AlignRight);
    date->setAlignment(Qt::AlignRight);
    quantity->setAlignment(Qt::AlignRight);

    orderNumLine = new QLineEdit(this);
    orderCkLine = new QComboBox(this);
    orderPkLine = new QComboBox(this);
    orderDate = new QDateEdit(this);
    orderQuantity = new QLineEdit(this);

    orderNumLine->setGeometry(140,130,130,20);
    orderCkLine->setGeometry(140,160,130,20);
    orderPkLine->setGeometry(140,190,130,20);
    orderDate->setGeometry(140,220,130,20);
    orderQuantity->setGeometry(140,250,130,20);

    orderNumLine->setReadOnly(true);
    orderQuantity->setAlignment(Qt::AlignRight);

    clearButton = new QPushButton(tr("CLEAR"), this);
    editButton = new QPushButton(tr("EDIT"), this);

    clearButton->setGeometry(20,320,115,115);
    editButton->setGeometry(155,320,115,115);

    // Connecting Signal and Slot
    connect(searchButton, SIGNAL(clicked()), SLOT(isOrderNum()));
    connect(clearButton, SIGNAL(clicked()), SLOT(clear()));
    connect(editButton, SIGNAL(clicked()), SLOT(sendEdit()));
}

// Receive CustomerKey for Edit from OrderManager
void OrderEdit::recvCustomerKey(QString ck, bool check)
{
    int index = orderCkLine->count();
    if (check) { // What is True means that New Customer was appended
        orderCkLine->insertItem(index, ck);
    } else if (!check){ // WHat is False means that Customer was deleted
        index = orderCkLine->findText(ck);
        orderCkLine->removeItem(index);
    }
}

// Receive ProductKey for Edit from OrderManager
void OrderEdit::recvProductKey(QString pk, bool check)
{
    int index = orderPkLine->count();
    if (check) { // What is True means that New Product was appended
        orderPkLine->insertItem(index, pk);
    } else if (!check){ // WHat is False means that Customer was deleted
        index = orderPkLine->findText(pk);
        orderPkLine->removeItem(index);
    }
}

// Slot connected to Clicked() of ClearButton
void OrderEdit::clear()
{
    orderNumLine->clear();
    orderCkLine->setCurrentIndex(0);
    orderPkLine->setCurrentIndex(0);
    orderDate->setDate(QDate::currentDate());
    orderQuantity->clear();
}

// Slot connected to Clicked() of SearchButton and emit signal sendOrderNum()
void OrderEdit::isOrderNum()
{
    // Send inputted OrderNumber to OrderManager for checking
   emit sendOrderNum(searchLine->text());
}

// Receive the Search Result from OrderManager
void OrderEdit::recvResult
(int recvNumber, int recvCk, int recvPk, QString recvDate, int recvQuantity, int total)
{
    Q_UNUSED(total);

    orderNumLine->setText(QString::number(recvNumber));
    orderCkLine->setCurrentIndex(orderCkLine->findText(QString::number(recvCk), Qt::MatchExactly));
    orderPkLine->setCurrentIndex(orderPkLine->findText(QString::number(recvPk), Qt::MatchExactly));
    orderDate->setDate(QDate::fromString(recvDate));
    orderQuantity->setText(QString::number(recvQuantity));
}

// Slot connected to Clicked() and emit signal resultEdit()
void OrderEdit::sendEdit()
{
    // Checking whether user inputted all of arguments
    if (orderNumLine->text().length() && orderQuantity->text().length()) {
        // Send inputted result for edit to OrderManager
        emit resultEdit(orderNumLine->text(), orderCkLine->currentText(),
                orderPkLine->currentText(), orderDate->date().toString(),
                    orderQuantity->text());
        QMessageBox edit;
        edit.button(QMessageBox::Ok);
        edit.setText(tr("Edit Succeed"));
        edit.setWindowTitle(tr("Edit Succeed"));
        edit.exec();
    } else {
        QMessageBox fail;
        fail.button(QMessageBox::Ok);
        fail.setText(tr("Please Fill all arguments"));
        fail.setWindowTitle(tr("Fill all things!"));
        fail.exec();
    }
}
