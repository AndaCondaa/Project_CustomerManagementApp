/*
 *  Program Name    :  Admin
 *  File Name       :  productinput.cpp
 *  Description     :  신규 상품을 입력하는 위젯
 *                      -> ProductManager위젯으로 정보를 전달하여, 새로운 상품객체 생성
*/


#include "productinput.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

ProductInput::ProductInput(QWidget *parent) :
    QWidget(parent)
{
    // Setting GUI
    setFixedSize(290, 340);

    pk = new QLabel(tr("ProductKey"), this);
    type = new QLabel(tr("Type"), this);
    name = new QLabel(tr("Name"), this);
    price = new QLabel(tr("Price"), this);

    pk->setGeometry(10,20,110,30);
    type->setGeometry(10,50,110,30);
    name->setGeometry(10,80,110,30);
    price->setGeometry(10,110,110,30);

    pk->setAlignment(Qt::AlignRight);
    type->setAlignment(Qt::AlignRight);
    name->setAlignment(Qt::AlignRight);
    price->setAlignment(Qt::AlignRight);

    pkLine = new QLineEdit(tr("Randomly created"), this);
    typeLine = new QComboBox(this);
    nameLine = new QLineEdit(this);
    priceLine = new QLineEdit(this);

    typeLine->addItem(tr("IMPLANT"));
    typeLine->addItem(tr("SCANNER"));
    typeLine->addItem(tr("CHAIR"));
    typeLine->addItem(tr("ETC"));

    pkLine->setGeometry(140,20,130,20);
    typeLine->setGeometry(140,50,130,20);
    nameLine->setGeometry(140,80,130,20);
    priceLine->setGeometry(140,110,130,20);

    pkLine->setReadOnly(true);
    pkLine->setAlignment(Qt::AlignRight);
    nameLine->setAlignment(Qt::AlignRight);
    priceLine->setAlignment(Qt::AlignRight);

    clearButton = new QPushButton(tr("CLEAR"), this);
    inputButton = new QPushButton(tr("INPUT"), this);

    clearButton->setGeometry(20,205,115,115);
    inputButton->setGeometry(155,205,115,115);

    // Connecting signals and slots
    connect(clearButton, SIGNAL(clicked()), SLOT(clear()));
    connect(inputButton, SIGNAL(clicked()), SLOT(inputEmit()));
}

// Slot connected to Clicked() of ClearButton
void ProductInput::clear()
{
    nameLine->clear();
    priceLine->clear();
}

// Slot connected to Clicked() of InputButton
void ProductInput::inputEmit()
{
    // Send inputted result to ProductManager for checking
    emit input(typeLine->currentText(), nameLine->text(),
               priceLine->text());
    nameLine->clear();
    priceLine->clear();
}
