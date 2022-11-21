#include "productinput.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QTime>
#include <QRandomGenerator>
#include <QSqlQuery>
#include <QSqlError>

ProductInput::ProductInput(QWidget *parent) :
    QWidget(parent)
{
    // Setting GUI
    setFixedSize(290, 340);
    setWindowTitle(tr("PRODUCT INPUT"));

    pk = new QLabel(tr("ProductKey"), this);
    type = new QLabel(tr("Type"), this);
    name = new QLabel(tr("Name"), this);
    price = new QLabel(tr("Price"), this);
    stock = new QLabel(tr("Stock"), this);

    pk->setGeometry(10,20,110,30);
    type->setGeometry(10,50,110,30);
    name->setGeometry(10,80,110,30);
    price->setGeometry(10,110,110,30);
    stock->setGeometry(10,140,110,30);

    pk->setAlignment(Qt::AlignRight);
    type->setAlignment(Qt::AlignRight);
    name->setAlignment(Qt::AlignRight);
    price->setAlignment(Qt::AlignRight);
    stock->setAlignment(Qt::AlignRight);

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
    nameLine->setAlignment(Qt::AlignRight);
    priceLine->setAlignment(Qt::AlignRight);
    stockLine->setAlignment(Qt::AlignRight);

    nameLine->setInputMask("NNNNNNNNNN;_");
    priceLine->setInputMask("000000;_");
    stockLine->setInputMask("00000;_");

    clearButton = new QPushButton(tr("CLEAR"), this);
    inputButton = new QPushButton(tr("INPUT"), this);

    clearButton->setGeometry(20,205,115,115);
    inputButton->setGeometry(155,205,115,115);

    // Connecting signals and slots
    connect(clearButton, SIGNAL(clicked()), SLOT(clear()));
    connect(inputButton, SIGNAL(clicked()), SLOT(input()));

    // Setting the TypeComboBox by getting information from product_type(table)
    QSqlDatabase productDB = QSqlDatabase::database("ProductManager");
    QSqlQuery query(productDB);
    query.exec("SELECT * FROM sys.PRODUCT_TYPE ORDER BY TYPE_ID");
    while (query.next())
         typeBox->addItem(query.value(1).toString());
}

// Receive current ProductKey for setting new ProductKey
void ProductInput::recvCurrentPK(int pk)
{
    index = pk / 100;
    if (index == 0)
        index = 9;
}

// Making new ProductKey by combining TypeID & Time & Index
int ProductInput::makeProductKey()
{
    int typeID = typeBox->currentIndex() + 1;

    srand(QTime::currentTime().msecsSinceStartOfDay());     // Making random values every times
    int tmp = rand() % 9;

    index ++;
    int PK = (index * 100) + ((typeID % 10) * 10) + tmp;

    return PK;
}

// Slot connected to Clicked() of InputButton
void ProductInput::input()
{
    int pk = makeProductKey();
    int typeID = typeBox->currentIndex() + 1;
    QString name = nameLine->text();
    int price = priceLine->text().toInt();
    int stock = stockLine->text().toInt();

    QSqlDatabase productDB = QSqlDatabase::database("ProductManager");
    QSqlQuery inputQuery(productDB);
    inputQuery.prepare("CALL sys.INPUT_PRODUCT (:pk, :type_id, :name, :price, :stock)");
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

// Slot connected to Clicked() of ClearButton
void ProductInput::clear()
{
    typeBox->setCurrentIndex(0);
    nameLine->clear();
    priceLine->clear();
    stockLine->clear();
}

