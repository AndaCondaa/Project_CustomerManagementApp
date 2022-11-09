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
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>

CustomerInput::CustomerInput(QWidget *parent)
    : QWidget{parent}
{
    setFixedSize(290, 340);

    //Setting GUI
    ckLabel = new QLabel(tr("CustomerKey"), this);
    clinicLabel = new QLabel(tr("Clinic"), this);
    licenseLabel = new QLabel(tr("License"), this);
    dentistLabel = new QLabel(tr("Dentist"), this);
    numberLabel = new QLabel(tr("Number"), this);

    ckLabel->setGeometry(10,20,90,30);
    clinicLabel->setGeometry(10,50,90,30);
    licenseLabel->setGeometry(10,80,90,30);
    dentistLabel->setGeometry(10,110,90,30);
    numberLabel->setGeometry(10,140,90,30);

    ckLabel->setAlignment(Qt::AlignRight);
    clinicLabel->setAlignment(Qt::AlignRight);
    licenseLabel->setAlignment(Qt::AlignRight);
    dentistLabel->setAlignment(Qt::AlignRight);
    numberLabel->setAlignment(Qt::AlignRight);

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

    licenseLine->setInputMask("00-0000-00");
    numberLine->setInputMask("000-0000-0000");

    clearButton = new QPushButton(tr("CLEAR"), this);
    inputButton = new QPushButton(tr("INPUT"), this);

    clearButton->setGeometry(20,205,115,115);
    inputButton->setGeometry(155,205,115,115);

    // Connecting Signal and Slot
    connect(clearButton, SIGNAL(clicked()), SLOT(clear()));
    connect(inputButton, SIGNAL(clicked()), SLOT(input()));
}

void CustomerInput::recvCurrentCK(int ck)
{
    index = ck / 10000;
}

// Make CustomerKey by using index, license, number
int CustomerInput::makeCustomerKey(QString license, QString number)
{
    QString tmp_license = license.split("-")[0] +
            license.split("-")[1] + license.split("-")[2];
    QString tmp_number = number.split("-")[0] +
            number.split("-")[1] + number.split("-")[2];

    // string to integer
    int temp1 = (tmp_license.toULongLong() * 111) % 10000;
    int temp2 = (tmp_number.toULongLong() * 111) % 10000;

    // making CustomerKey
    index += 1;
    int CK = (index * 10000) + (temp1 + temp2) % 10000;     // 만의 자리 값이 고객등록 순서를 의미

    return CK;
}

// Slot connected to Clicked() of ClearButton
void CustomerInput::clear()
{
    clinicLine->clear();
    licenseLine->clear();
    dentistLine->clear();
    numberLine->clear();
}

void CustomerInput::input()
{

    int ck = makeCustomerKey(licenseLine->text(), numberLine->text());
    QString clinic = clinicLine->text();
    QString license = licenseLine->text();
    QString dentist = dentistLine->text();
    QString number = numberLine->text();

//    QSqlDatabase db = QSqlDatabase::database();
//    QSqlQuery query(db);
    QSqlQuery query;
    query.prepare("INSERT INTO CUSTOMER(CUSTOMER_KEY, CLINIC_NAME, LICENSE_NUMBER, DENTIST_NAME, PHONE_NUMBER, ORDER_AMOUNT)"
                  "VALUES  (:ck, :clinic, :license, :dentist, :number, :amount);");
    query.setForwardOnly(true);
    query.bindValue(":ck", ck);
    query.bindValue(":clinic", clinic);
    query.bindValue(":license", license);
    query.bindValue(":dentist", dentist);
    query.bindValue(":number", number);
    query.bindValue(":amount", 0);
    bool isExec = query.exec();

    if (isExec) {
        clear();
        qDebug() << "성공";
        emit inputCustomer();
    }
    else {
        if (query.lastError().text().contains("ORA-00001")) {
            qDebug() << "유니크 에러";
        } else if (query.lastError().text().contains("ORA-12899"))
            qDebug() << "컬럼 데이터 초과 에러";

    }
}
