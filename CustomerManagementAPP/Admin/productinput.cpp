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
#include <QTime>
#include <QRandomGenerator>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

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
    typeBox = new QComboBox(this);
    nameLine = new QLineEdit(this);
    priceLine = new QLineEdit(this);
    stockLine = new QLineEdit(this);

    pkLine->setGeometry(140,20,130,20);
    typeBox->setGeometry(140,50,130,20);
    nameLine->setGeometry(140,80,130,20);
    priceLine->setGeometry(140,110,130,20);
    stockLine->setGeometry(140,140,130,20);

    pkLine->setReadOnly(true);
    pkLine->setAlignment(Qt::AlignRight);
    nameLine->setAlignment(Qt::AlignRight);
    priceLine->setAlignment(Qt::AlignRight);
    stockLine->setAlignment(Qt::AlignRight);

    clearButton = new QPushButton(tr("CLEAR"), this);
    inputButton = new QPushButton(tr("INPUT"), this);

    clearButton->setGeometry(20,205,115,115);
    inputButton->setGeometry(155,205,115,115);

    // Connecting signals and slots
    connect(clearButton, SIGNAL(clicked()), SLOT(clear()));
    connect(inputButton, SIGNAL(clicked()), SLOT(input()));

    // 타입콤보박스 세팅 -> 타입테이블에서 가져오기
    QSqlQuery query;
    query.exec("SELECT * FROM PRODUCT_TYPE ORDER BY TYPE_ID");

    QSqlRecord record = query.record();
    int nameCol = record.indexOf("TYPE_NAME");
    while (query.next())
         typeBox->addItem(query.value(nameCol).toString());
}

// Slot connected to Clicked() of ClearButton
void ProductInput::clear()
{
    nameLine->clear();
    priceLine->clear();
}

void ProductInput::recvCurrentPK(int pk)
{
    index = pk / 100;
    if (index == 0)
        index = 9;
}

int ProductInput::makeProductKey(int typeID)
{

    srand(QTime::currentTime().msecsSinceStartOfDay()); //시드를 주어 매번 랜덤한 값으로 나타냄
    int tmp = rand() % 9;

    index ++;
    int PK = (index * 100) + ((typeID % 10) * 10) + tmp;


    return PK;
}

void ProductInput::input()
{
    int pk = makeProductKey(typeBox->currentIndex() + 1);
    int typeID = typeBox->currentIndex() + 1;
    QString name = nameLine->text();
    int price = priceLine->text().toInt();
    int stock = stockLine->text().toInt();

    QSqlQuery inputQuery;
    inputQuery.prepare("CALL INPUT_PRODUCT (:pk, :type_id, :name, :price, :stock)");
    inputQuery.bindValue(":pk", pk);
    inputQuery.bindValue(":type_id", typeID);
    inputQuery.bindValue(":name", name);
    inputQuery.bindValue(":price", price);
    inputQuery.bindValue(":stock", stock);
    bool isExec = inputQuery.exec();

    if (isExec) {
        clear();
        qDebug() << "성공";
        emit inputProduct();
    }
    else {
        if (inputQuery.lastError().text().contains("ORA-00001")) {
            qDebug() << "유니크 에러";
        } else if (inputQuery.lastError().text().contains("ORA-12899"))
            qDebug() << "컬럼 데이터 초과 에러";
    }


}
