/*
 *  Program Name    :  Admin
 *  File Name       :  productmanager.cpp
 *  Description     :  상품관리 위젯
 *                      -> 등록된 상품 목록 출력
 *                      -> 신규등록 요청 수행
 *                      -> 검색 요청 결과 전송
 *                      -> 정보수정 요청 수행
 *                      -> 주문클래스로 상품정보 전송
*/


#include "productmanager.h"
#include "ui_productmanager.h"
#include "product.h"
#include "productinput.h"
#include "productsearch.h"
#include "productedit.h"

#include <QFile>
#include <QMessageBox>

ProductManager::ProductManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProductManager)
{
    ui->setupUi(this);

    productInput = new ProductInput;
    productSearch = new ProductSearch;
    productEdit = new ProductEdit;

    // Connecting Signal and Slot for Input
    connect(productInput, SIGNAL(input(QString, QString, QString)),
            this, SLOT(inputData(QString, QString, QString)));

    // Connecting Signal and Slot for Search
    connect(productSearch, SIGNAL(search(int, QString)),
            this, SLOT(checkSearch(int, QString)));
    connect(this, SIGNAL(searchResult(QString, QString, QString, QString)),
            productSearch, SLOT(recvSearchResult(QString, QString, QString, QString)));

    // Connecting Signal and Slot for Edit
    connect(productEdit, SIGNAL(sendPK(QString)), SLOT(recvPK(QString)));
    connect(this, SIGNAL(searchResult(QString, QString, QString, QString)),
            productEdit, SLOT(recvResult(QString, QString, QString, QString)));
    connect(productEdit, SIGNAL(resultEdit(QString, QString, QString, QString)),
            SLOT(recvEditResult(QString, QString, QString, QString)));
}

ProductManager::~ProductManager()
{
    delete ui;

    //Data Saving
    QFile file("../Admin/data/manage/product_list.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : productList) {
        Product* p = v;
        out << p->getProductKey() << ", ";
        out << p->getType() << ", ";
        out << p->getProductName() << ", ";
        out << p->getPrice() << "\n";
    }
    file.close( );
}

// Load registerd Product List
void ProductManager::loadData()
{
    //Data Loading
    QFile file("../Admin/data/manage/product_list.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size()) {
            int index = makeIndex();
            Product* product = new Product(row[0].toInt(), row[1],
                    row[2], row[3].toInt());
            ui->productTreeWidget->addTopLevelItem(product);
            productList.insert(index, product);
            emit sendProductKey(row[0], row[3], true);
        }
    }
    file.close( );
}

// Make Index for Key of Map(ProductList)
int ProductManager::makeIndex()
{
    if (productList.size() == 0) {
        return 1;
    }
    else {
        auto c = productList.end();
        c--;
        int index = (c.value()->getProductKey()) / 10000;
        return ++index;
    }
}

// Make CustomerKey by using index, Type, price
int ProductManager::makeProductKey(int index, QString type, int price)
{

    int PK;
    index *=10000;

    if (type == "IMPLANT")
        PK = 1000;
    else if (type == "SCANNER")
        PK = 2000;
    else if (type == "CHAIR")
        PK = 3000;
    else
        PK = 4000;

    PK += (price / 10000) % 1000;
    PK += index;

    return PK;
}

// Show the ProductInput Widget
void ProductManager::on_inputButton_clicked()
{
    productInput->show();
}

// Show the ProductSearch Widget
void ProductManager::on_searchButton_clicked()
{
    productSearch->show();
}

// Show the ProductEdit Widget
void ProductManager::on_editButton_clicked()
{
    productEdit->show();
}

// Slot for Remove Item (Product)
void ProductManager::on_productTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    QString pk = item->text(0);
    QString type = item->text(1);
    QString name = item->text(2);

    // Check that user remove the customer really
    QMessageBox remove;
    remove.setIcon(QMessageBox::Warning);
    remove.setText(pk + " " + type + " " + name + " REMOVE?");
    remove.addButton(tr("YES"), QMessageBox::ActionRole);
    remove.addButton(tr("NO"), QMessageBox::ActionRole);
    remove.exec();

    // Remove
    if (remove.clickedButton()->text() == "YES") {
        foreach (auto removeItem, productList) {
            if (removeItem->getProductKey() == pk.toInt()) {
                //Send ProductKey to OrderManager and ChatManager
                emit sendProductKey(removeItem->text(0), removeItem->text(3), false);
                productList.remove(productList.key(removeItem));
        }
        ui->productTreeWidget->takeTopLevelItem(ui->productTreeWidget->indexOfTopLevelItem(item));
        ui->productTreeWidget->update();
        }
    }
}

// Receive the data about new Product's information from ProductInput
// Add new Product
void ProductManager::inputData
            (QString type, QString name, QString price)
{
    int index = makeIndex();
    int pk = makeProductKey(index, type, price.toInt());

    Product* product = new Product
            (pk, type, name, price.toInt());

    ui->productTreeWidget->addTopLevelItem(product);
    productList.insert(index, product);

    // Send ProductKey to OrderManager and ChatManager
    emit sendProductKey(QString::number(pk), price, true);
}

// Receive ProductKey from ProductSearch for search
void ProductManager::checkSearch(int index, QString input)
{
    int op = 0;
    searchList.clear();
    switch (index) {
    case 0:
        for(auto c = productList.begin() ; c != productList.end(); c++) {
            if (QString::number(c.value()->getProductKey()).contains(input)) {
                checkSearch(c.key());
                op++;
            }
        }
        break;
    case 1:
        for(auto c = productList.begin() ; c != productList.end(); c++) {
            if (c.value()->getType().contains(input, Qt::CaseInsensitive)) {
                checkSearch(c.key());
                op++;
            }
        }
        break;
    case 2:
        for(auto c = productList.begin() ; c != productList.end(); c++) {
            if (c.value()->getProductName().contains(input, Qt::CaseInsensitive)) {
                checkSearch(c.key());
                op++;
            }
        }
        break;
    case 3:
        for(auto c = productList.begin() ; c != productList.end(); c++) {
            if (QString::number(c.value()->getPrice()).contains(input, Qt::CaseInsensitive)) {
                checkSearch(c.key());
                op++;
            }
        }
        break;
    }
    if (op == 0) {
        QMessageBox nothing;
        nothing.button(QMessageBox::Ok);
        nothing.setText("No Result");
        nothing.setWindowTitle("No Result");
        nothing.exec();
    }
}

// Check ProductKey and emit signal searchResult()
void ProductManager::checkSearch(int key)
{
    Product* product = productList.value(key);

    // Send search result to ProductSearch and ProductEdit
    emit searchResult(QString::number(product->getProductKey()), product->getType(),
                      product->getProductName(), QString::number(product->getPrice()));
}

// Receive the ProductKey from ProductEdit for check
void ProductManager::recvPK(QString pk)
{
    foreach (auto item, ui->productTreeWidget->findItems(pk, Qt::MatchExactly, 0)) {
        // Send search result to ProductSearch and ProductEdit
        emit searchResult(item->text(0), item->text(1),
                            item->text(2), item->text(3));
    }
    if ((ui->productTreeWidget->findItems(pk, Qt::MatchExactly, 0)).isEmpty()) {
        QMessageBox nothing;
        nothing.button(QMessageBox::Ok);
        nothing.setText("No Result");
        nothing.setWindowTitle("No Result");
        nothing.exec();
    }
}

// Receive the result of edit from ProductEdit
void ProductManager::recvEditResult
(QString pk, QString type, QString name, QString price)
{
    foreach (auto item, productList.values()) {
        if (item->getProductKey() == pk.toInt()) {
            item->setType(type);
            item->setProductName(name);
            item->setPrice(price.toInt());
            break;
        }
    }
    emit changePrice(pk, price);        // Send Price changed to OrderManager
}






