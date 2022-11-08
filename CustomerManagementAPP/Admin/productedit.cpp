/*
 *  Program Name    :  Admin
 *  File Name       :  productedit.cpp
 *  Description     :  상품 정보를 수정하는 위젯
*/


#include "productedit.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>

ProductEdit::ProductEdit(QWidget *parent)
    : QWidget{parent}
{
    // Setting GUI
    setFixedSize(290, 420);

    searchProductKey = new QLabel(tr("PRODUCTKEY"), this);
    searchLine = new QLineEdit(this);
    searchButton = new QPushButton(tr("SEARCH"), this);

    searchProductKey->setGeometry(40,20,230,20);
    searchLine->setGeometry(40,50,230,20);
    searchButton->setGeometry(40,80,230,30);
    searchProductKey->setAlignment(Qt::AlignCenter);
    searchLine->setAlignment(Qt::AlignRight);

    pk = new QLabel("ProductKey", this);
    type = new QLabel("Type", this);
    name = new QLabel("Name", this);
    price = new QLabel("Price", this);

    pk->setGeometry(10,130,110,30);
    type->setGeometry(10,160,110,30);
    name->setGeometry(10,190,110,30);
    price->setGeometry(10,220,110,30);

    pk->setAlignment(Qt::AlignRight);
    type->setAlignment(Qt::AlignRight);
    name->setAlignment(Qt::AlignRight);
    price->setAlignment(Qt::AlignRight);

    pkLine = new QLineEdit(this);
    typeLine = new QComboBox(this);
    nameLine = new QLineEdit(this);
    priceLine = new QLineEdit(this);

    typeLine->addItem(tr("IMPLANT"));
    typeLine->addItem(tr("SCANNER"));
    typeLine->addItem(tr("CHAIR"));
    typeLine->addItem(tr("ETC"));

    pkLine->setGeometry(140,130,130,20);
    typeLine->setGeometry(140,160,130,20);
    nameLine->setGeometry(140,190,130,20);
    priceLine->setGeometry(140,220,130,20);

    pkLine->setReadOnly(true);
    pkLine->setAlignment(Qt::AlignRight);
    nameLine->setAlignment(Qt::AlignRight);
    priceLine->setAlignment(Qt::AlignRight);

    clearButton = new QPushButton("&CLEAR", this);
    editButton = new QPushButton("&EDIT", this);

    clearButton->setGeometry(20,290,115,115);
    editButton->setGeometry(155,290,115,115);

    // Connecting Signal and Slot
    connect(searchButton, SIGNAL(clicked()), SLOT(isPK()));
    connect(clearButton, SIGNAL(clicked()), SLOT(clear()));
    connect(editButton, SIGNAL(clicked()), SLOT(sendEdit()));

}

// Slot connected to Clicked() of ClearButton
void ProductEdit::clear()
{
    pkLine->clear();
    nameLine->clear();
    priceLine->clear();
}

// Slot connected to Clicked() of SearchButton and emit signal sendPk()
void ProductEdit::isPK()
{
     // Send inputted ProductKey to ProductManager for checking
    emit sendPK(searchLine->text());
}

// Receive the Search Result from ProductManager
void ProductEdit::recvResult
(QString pk, QString type, QString name, QString price)
{
    pkLine->setText(pk);
    if (type == "IMPLANT")
        typeLine->setCurrentIndex(0);
    else if (type == "SCANNER")
        typeLine->setCurrentIndex(1);
    else if (type == "CHAIR")
            typeLine->setCurrentIndex(2);
    else
        typeLine->setCurrentIndex(3);
    nameLine->setText(name);
    priceLine->setText(price);
}

// Slot connected to Clicked() and emit signal resultEdit()
void ProductEdit::sendEdit()
{
    // Checking whether user inputted all of arguments
    if (pkLine->text().length() && nameLine->text().length() &&
            priceLine->text().length()) {
        // Send inputted result for edit to ProductManager
        emit resultEdit(pkLine->text(), typeLine->currentText(),
                nameLine->text(), priceLine->text());

        QMessageBox edit;
        edit.button(QMessageBox::Ok);
        edit.setText(tr("Edit Succeed"));
        edit.setWindowTitle(tr("Edit Succeed"));
        edit.exec();

        pkLine->clear();
        nameLine->clear();
        priceLine->clear();
    } else {
        QMessageBox fail;
        fail.button(QMessageBox::Ok);
        fail.setText(tr("Please Fill all arguments"));
        fail.setWindowTitle(tr("Fill all things!"));
        fail.exec();
    }
}
