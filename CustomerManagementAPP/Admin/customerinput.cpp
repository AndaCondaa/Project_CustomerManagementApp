/*
 *  Program Name    :  Admin
 *  File Name       :  customerinput.cpp
 *  Description     :  새로운 고객을 입력하는 위젯
 *                      -> CustomerManager위젯으로 정보를 전달하여, 새로운 고객객체 생성
*/


#include "customerinput.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

CustomerInput::CustomerInput(QWidget *parent)
    : QWidget{parent}
{
    setFixedSize(290, 340);

    //Setting GUI
    ck = new QLabel("CustomerKey", this);
    clinic = new QLabel("Clinic", this);
    license = new QLabel("License", this);
    dentist = new QLabel("Dentist", this);
    number = new QLabel("Number", this);

    ck->setGeometry(10,20,110,30);
    clinic->setGeometry(10,50,110,30);
    license->setGeometry(10,80,110,30);
    dentist->setGeometry(10,110,110,30);
    number->setGeometry(10,140,110,30);

    ck->setAlignment(Qt::AlignRight);
    clinic->setAlignment(Qt::AlignRight);
    license->setAlignment(Qt::AlignRight);
    dentist->setAlignment(Qt::AlignRight);
    number->setAlignment(Qt::AlignRight);

    ckLine = new QLineEdit("Randomly created", this);
    clinicLine = new QLineEdit(this);
    licenseLine = new QLineEdit(this);
    dentistLine = new QLineEdit(this);
    numberLine = new QLineEdit(this);

    ckLine->setGeometry(140,20,130,20);
    clinicLine->setGeometry(140,50,130,20);
    licenseLine->setGeometry(140,80,130,20);
    dentistLine->setGeometry(140,110,130,20);
    numberLine->setGeometry(140,140,130,20);

    ckLine->setReadOnly(true);
    ckLine->setAlignment(Qt::AlignRight);
    clinicLine->setAlignment(Qt::AlignRight);
    licenseLine->setAlignment(Qt::AlignRight);
    dentistLine->setAlignment(Qt::AlignRight);
    numberLine->setAlignment(Qt::AlignRight);

    clearButton = new QPushButton(tr("CLEAR"), this);
    inputButton = new QPushButton(tr("INPUT"), this);

    clearButton->setGeometry(20,205,115,115);
    inputButton->setGeometry(155,205,115,115);

    // Connecting Signal and Slot
    connect(clearButton, SIGNAL(clicked()), SLOT(clear()));
    connect(inputButton, SIGNAL(clicked()), SLOT(inputEmit()));
}

// Slot connected to Clicked() of ClearButton
void CustomerInput::clear()
{
    clinicLine->clear();
    licenseLine->clear();
    dentistLine->clear();
    numberLine->clear();
}

// Slot connected to Clicked() of InputButton
void CustomerInput::inputEmit()
{
    // Send inputted result to CustomerManager for checking
    emit input(clinicLine->text(), licenseLine->text(),
               dentistLine->text(), numberLine->text());
    clinicLine->clear();
    licenseLine->clear();
    dentistLine->clear();
    numberLine->clear();
}

