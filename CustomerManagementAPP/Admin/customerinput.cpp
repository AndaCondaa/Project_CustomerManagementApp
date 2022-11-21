
#include "customerinput.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTime>
#include <QRandomGenerator>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>

CustomerInput::CustomerInput(QWidget *parent)
    : QWidget{parent}
{
    setFixedSize(290, 340);
    setWindowTitle(tr("CUSTOMER INPUT"));

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

    clinicLine->setInputMask("NNNNNNNNNN;_");
    licenseLine->setInputMask("00-0000-00;_");
    dentistLine->setInputMask("NNNNNNNNNN;_");
    numberLine->setInputMask("000-0000-0000;_");

    clearButton = new QPushButton(tr("CLEAR"), this);
    inputButton = new QPushButton(tr("INPUT"), this);

    clearButton->setGeometry(20,205,115,115);
    inputButton->setGeometry(155,205,115,115);

    // Connecting Signal and Slot
    connect(clearButton, SIGNAL(clicked()), SLOT(clear()));
    connect(inputButton, SIGNAL(clicked()), SLOT(input()));
}

// Receive current customerKey for setting new CustomerKey
void CustomerInput::recvCurrentCK(int ck)
{
    index = ck / 1000;
    if (index == 0)
        index = 99;
}

// Make CustomerKey by using index, license, number
int CustomerInput::makeCustomerKey()
{
    int license = (licenseLine->text().split("-")[0] +
            licenseLine->text().split("-")[1] + licenseLine->text().split("-")[2]).toInt();

    srand(QTime::currentTime().msecsSinceStartOfDay());  // making random values every times
    int tmp = (rand() % 9) * 100;
    int tmp_license = (license * 111) % 100;

    index++;
    int CK = (index * 1000) + tmp + tmp_license;

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

// Slot connected to Clicked() of InputButton
void CustomerInput::input()
{
    int ck = makeCustomerKey();
    QString clinic = clinicLine->text();
    QString license = licenseLine->text();
    QString dentist = dentistLine->text();
    QString number = numberLine->text();

    QSqlDatabase customerDB = QSqlDatabase::database("CustomerManager");
    QSqlQuery inputQuery(customerDB);
    inputQuery.prepare("CALL sys.INPUT_CUSTOMER (:ck, :clinic, :license, :dentist, :number, :amount)");
    inputQuery.setForwardOnly(true);
    inputQuery.bindValue(":ck", ck);
    inputQuery.bindValue(":clinic", clinic);
    inputQuery.bindValue(":license", license);
    inputQuery.bindValue(":dentist", dentist);
    inputQuery.bindValue(":number", number);
    inputQuery.bindValue(":amount", 0);
    bool isExec = inputQuery.exec();

    if (isExec) {
        clear();
        qDebug() << "성공";
        emit inputCustomer();
    }
    else {
        if (inputQuery.lastError().text().contains("ORA-00001")) {
            qDebug() << "유니크 에러";
        } else if (inputQuery.lastError().text().contains("ORA-12899"))
            qDebug() << "컬럼 데이터 초과 에러";
    }
}
