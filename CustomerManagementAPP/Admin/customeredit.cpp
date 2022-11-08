/*
 *  Program Name    :  Admin
 *  File Name       :  customeredit.cpp
 *  Description     :  고객 정보를 수정하는 위젯
*/


#include "customeredit.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>

CustomerEdit::CustomerEdit(QWidget *parent)
    : QWidget{parent}
{
    // GUI Setting
    setFixedSize(290, 450);

    customerKey = new QLabel("CUSTOMERKEY", this);
    searchLine = new QLineEdit(this);
    searchButton = new QPushButton(tr("SEARCH"), this);

    customerKey->setGeometry(40,20,230,20);
    searchLine->setGeometry(40,50,230,20);
    searchButton->setGeometry(40,80,230,30);
    customerKey->setAlignment(Qt::AlignCenter);
    searchLine->setAlignment(Qt::AlignRight);

    ck = new QLabel(tr("CustomerKey"), this);
    clinic = new QLabel(tr("Clinic"), this);
    license = new QLabel(tr("License"), this);
    dentist = new QLabel(tr("Dentist"), this);
    number = new QLabel(tr("Number"), this);

    ck->setGeometry(10,130,110,30);
    clinic->setGeometry(10,160,110,30);
    license->setGeometry(10,190,110,30);
    dentist->setGeometry(10,220,110,30);
    number->setGeometry(10,250,110,30);

    ck->setAlignment(Qt::AlignRight);
    clinic->setAlignment(Qt::AlignRight);
    license->setAlignment(Qt::AlignRight);
    dentist->setAlignment(Qt::AlignRight);
    number->setAlignment(Qt::AlignRight);

    ckLine = new QLineEdit(this);
    clinicLine = new QLineEdit(this);
    licenseLine = new QLineEdit(this);
    dentistLine = new QLineEdit(this);
    numberLine = new QLineEdit(this);

    ckLine->setGeometry(140,130,130,20);
    clinicLine->setGeometry(140,160,130,20);
    licenseLine->setGeometry(140,190,130,20);
    dentistLine->setGeometry(140,220,130,20);
    numberLine->setGeometry(140,250,130,20);

    ckLine->setReadOnly(true);
    ckLine->setAlignment(Qt::AlignRight);
    clinicLine->setAlignment(Qt::AlignRight);
    licenseLine->setAlignment(Qt::AlignRight);
    dentistLine->setAlignment(Qt::AlignRight);
    numberLine->setAlignment(Qt::AlignRight);

    clearButton = new QPushButton(tr("CLEAR"), this);
    editButton = new QPushButton(tr("EDIT"), this);

    clearButton->setGeometry(20,320,115,115);
    editButton->setGeometry(155,320,115,115);

    // Connecting Signal and Slot
    connect(searchButton, SIGNAL(clicked()), SLOT(isCK()));
    connect(clearButton, SIGNAL(clicked()), SLOT(clear()));
    connect(editButton, SIGNAL(clicked()), SLOT(sendEdit()));
}

// Slot connected to Clicked() of ClearButton
void CustomerEdit::clear()
{
    ckLine->clear();
    clinicLine->clear();
    licenseLine->clear();
    dentistLine->clear();
    numberLine->clear();
}

// Slot connected to Clicked() of SearchButton and emit signal sendCK()
void CustomerEdit::isCK()
{
    // Send inputted CustomerKey to CustomerManager for checking
   emit sendCK(searchLine->text());
}

// Send inputted result for edit to CustomerManager
void CustomerEdit::recvSearchForEdit
(QString ck, QString clinic, QString license, QString dentist, QString number)
{
    ckLine->setText(ck);
    clinicLine->setText(clinic);
    licenseLine->setText(license);
    dentistLine->setText(dentist);
    numberLine->setText(number);
}

// Slot connected to Clicked() and emit signal resultEdit()
void CustomerEdit::sendEdit()
{
    // Checking whether user inputted all of arguments
    if (ckLine->text().length() && clinicLine->text().length() &&
        licenseLine->text().length() && dentistLine->text().length() &&
            numberLine->text().length()) {
        // Send inputted result for edit to CustomerManager
        emit resultEdit(ckLine->text(), clinicLine->text(),
                licenseLine->text(), dentistLine->text(),
                    numberLine->text());
        QMessageBox edit;
        edit.button(QMessageBox::Ok);
        edit.setText(tr("Edit Succeed"));
        edit.setWindowTitle(tr("Edit Succeed"));
        edit.exec();
    } else { // if user didn't fill any arguments, Show the MessageBox
        QMessageBox fill;
        fill.button(QMessageBox::Ok);
        fill.setText(tr("Please Fill all arguments"));
        fill.setWindowTitle(tr("Fill all things!"));
        fill.exec();
    }
}
