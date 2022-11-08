/*
 *  Program Name    :  Admin
 *  File Name       :  ordermanager.cpp
 *  Description     :  주문관리 위젯
 *                      -> 등록된 주문 목록 출력
 *                      -> 신규주문 요청 수행
 *                      -> 검색 요청 결과 전송
 *                      -> 정보수정 요청 수행
*/


#include "ordermanager.h"
#include "ui_ordermanager.h"

#include "order.h"
#include "orderinput.h"
#include "ordersearch.h"
#include "orderedit.h"

#include <QFile>
#include <QMessageBox>

OrderManager::OrderManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderManager)
{
    ui->setupUi(this);
    orderInput = new OrderInput;
    orderSearch = new OrderSearch;
    orderEdit = new OrderEdit;

    //Data Loading
    QFile file("../Admin/data/manage/order_list.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size()) {
            int index = makeIndex();
            Order *order = new Order(row[0].toInt(), row[1].toInt(),
                    row[2].toInt(), row[3], row[4].toInt(),
                    row[5].toInt());
            ui->orderTreeWidget->addTopLevelItem(order);
            orderList.insert(index, order);
        }
    }
    file.close( );

    // Connecting Signal and Slot for Input
    connect(this, SIGNAL(sendCustomerKey(QString, bool)),
            orderInput, SLOT(recvCustomerKey(QString, bool)));
    connect(this, SIGNAL(sendProductKey(QString, bool)),
            orderInput, SLOT(recvProductKey(QString, bool)));
    connect(orderInput, SIGNAL(input(QString, QString, QString, QString)),
            this, SLOT(inputData(QString, QString, QString, QString)));

    // Connecting Signal and Slot for Search
    connect(orderSearch, SIGNAL(search(int, QString)),
            this, SLOT(checkSearch(int, QString)));
    connect(this, SIGNAL(searchResult(int, int, int, QString, int, int)),
            orderSearch, SLOT(recvSearchResult(int, int, int, QString, int, int)));

    // Connecting Signal and Slot for Edit
    // ComboBox setting
    connect(this, SIGNAL(sendCustomerKey(QString, bool)),
            orderEdit, SLOT(recvCustomerKey(QString, bool)));
    connect(this, SIGNAL(sendProductKey(QString, bool)),
            orderEdit, SLOT(recvProductKey(QString, bool)));

    // Edit Actions
    connect(orderEdit, SIGNAL(sendOrderNum(QString)), SLOT(recvOrderNumber(QString)));
    connect(this, SIGNAL(searchResult(int, int, int, QString, int, int)),
            orderEdit, SLOT(recvResult(int, int, int, QString, int, int)));
    connect(orderEdit, SIGNAL(resultEdit(QString, QString, QString, QString, QString)),
            SLOT(recvEditResult(QString, QString, QString, QString, QString)));
}

OrderManager::~OrderManager()
{
    delete ui;

    //Data Saving
    QFile file("../Admin/data/manage/order_list.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : orderList) {
        Order *o = v;
        out << o->getOrderNum() << ", ";
        out << o->getOrderCK() << ", ";
        out << o->getOrderPK() << ", ";
        out << o->getDate() << ", ";
        out << o->getQuantity() << ", ";
        out << o->getTotal() << "\n";
    }
    file.close( );
}

// Make Index for Key of Map(OrderList)
int OrderManager::makeIndex()
{
    if (orderList.size() == 0) {
        return 1;
    }
    else {
        auto c = orderList.end();
        c--;
        int index = (c.value()->getOrderNum()) / 10000;
        return ++index;
    }
}

// Make OrderNumber by using Index, CustomerKey, ProductKey
int OrderManager::makeOrderNum(int index, int orderCK, int orderPK)
{
    int orderNum = 0;
    orderNum += (orderCK + orderPK) % 10000;
    orderNum += index * 10000;
    return orderNum;
}

// Calculate Total price
int OrderManager::makeTotal(int price, int quantity)
{
    int total = price * quantity;
    return total;
}

// Show the OrderInput Widget
void OrderManager::on_inputButton_clicked()
{
    orderInput->show();
}

// Show the OrderSearch Widget
void OrderManager::on_searchButton_clicked()
{
    orderSearch->show();
}

// Show the OrderEdit Widget
void OrderManager::on_editButton_clicked()
{
    orderEdit->show();
}

// Slot for Remove Item (Order)
void OrderManager::on_orderTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    QString orderNum = item->text(0);

    // Check that user remove the order really
    QMessageBox remove;
    remove.setIcon(QMessageBox::Warning);
    remove.setText(orderNum + " REMOVE?");
    remove.addButton(tr("YES"), QMessageBox::ActionRole);
    remove.addButton(tr("NO"), QMessageBox::ActionRole);
    remove.exec();

    // Remove
    if (remove.clickedButton()->text() == "YES") {
        foreach (auto removeItem, orderList) {
            if (removeItem->getOrderNum() == orderNum.toInt()) {
                orderList.remove(orderList.key(removeItem));
        }
        ui->orderTreeWidget->takeTopLevelItem(ui->orderTreeWidget->indexOfTopLevelItem(item));
        ui->orderTreeWidget->update();
        }
    }
}

// Receive Keys from CustomerManager and ProductManager for filling ComboBox
void OrderManager::recvCustomerKey(QString ck, QString clinic, bool check)
{
    Q_UNUSED(clinic);

    // Send CustomerKey to OrderInput and OrderEdit
    emit sendCustomerKey(ck, check);
}

void OrderManager::recvProductKey(QString pk, QString price, bool check)
{
    productPriceHash[pk] = price;

    // Send ProductKey to OrderInput and OrderEdit
    emit sendProductKey(pk, check);
}

// Receive new price from ProductManager and Edit Hash(productPriceHash)
void OrderManager::recvChangePrice(QString pk, QString price)
{
    productPriceHash.insert(pk, price);
}

// Receive the data about new Order's information from OrderInput
void OrderManager::inputData(QString ck, QString pk, QString date, QString quantity)
{
    int index = makeIndex();
    int orderNum = makeOrderNum(index, ck.toInt(), pk.toInt());
    int total = makeTotal(productPriceHash[pk].toInt(), quantity.toInt());
    Order *order = new Order(orderNum, ck.toInt(), pk.toInt(), date, quantity.toInt(), total);
    ui->orderTreeWidget->addTopLevelItem(order);
    orderList.insert(index, order);
}

// Receive the values applied search from OrderSearch
void OrderManager::checkSearch(int index, QString input)
{
    int op = 0;
    searchList.clear();
    switch (index) {
    case 0:
        for(auto c = orderList.begin() ; c != orderList.end(); c++) {
            if (QString::number(c.value()->getOrderNum()).contains(input)) {
                checkSearch(c.key());
                op++;
            }
        }
        break;
    case 1:
        for(auto c = orderList.begin() ; c != orderList.end(); c++) {
            if (QString::number(c.value()->getOrderCK()).contains(input)) {
                checkSearch(c.key());
                op++;
            }
        }
        break;
    case 2:
        for(auto c = orderList.begin() ; c != orderList.end(); c++) {
            if (QString::number(c.value()->getOrderPK()).contains(input)) {
                checkSearch(c.key());
                op++;
            }
        }
        break;
    case 3:
        for(auto c = orderList.begin() ; c != orderList.end(); c++) {
            if (c.value()->getDate().contains(input, Qt::CaseInsensitive)) {
                checkSearch(c.key());
                op++;
            }
        }
        break;
    case 4:
        for(auto c = orderList.begin() ; c != orderList.end(); c++) {
            if (QString::number(c.value()->getQuantity()).contains(input)) {
                checkSearch(c.key());
                op++;
            }
        }
        break;
    case 5:
        for(auto c = orderList.begin() ; c != orderList.end(); c++) {
            if (QString::number(c.value()->getTotal()).contains(input)) {
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

// Check OrderNumber and emit signal searchResult()
void OrderManager::checkSearch(int key)
{
    Order* order = orderList.value(key);

    // Send result of search to OrderSearch and OrderEdit
    emit searchResult(order->getOrderNum(), order->getOrderCK(),
                      order->getOrderPK(), order->getDate(),
                      order->getQuantity(), order->getTotal());
}

// Slots for Edit
// Receive the OrderNumber from OrderEdit for check
void OrderManager::recvOrderNumber(QString orderNumber)
{
    foreach (auto item, ui->orderTreeWidget->findItems(orderNumber, Qt::MatchExactly, 0)) {
        emit searchResult(item->text(0).toInt(), item->text(1).toInt(),
                          item->text(2).toInt(), item->text(3),
                          item->text(4).toInt(), item->text(5).toInt());
    }
    if ((ui->orderTreeWidget->findItems(orderNumber, Qt::MatchExactly, 0)).isEmpty()) {
        QMessageBox nothing;
        nothing.button(QMessageBox::Ok);
        nothing.setText("No Result");
        nothing.setWindowTitle("No Result");
        nothing.exec();
    }
}

// Receive the result of edit from OrderEdit
void OrderManager::recvEditResult
(QString orderNumber, QString ck, QString pk, QString recvDate, QString quantity)
{
    int total = makeTotal((productPriceHash[pk]).toInt(), quantity.toInt());

    // Add new Order
    foreach (auto item, orderList.values()) {
        if (item->getOrderNum() == orderNumber.toInt()) {
            item->setOrderCK(ck.toInt());
            item->setOrderPK(pk.toInt());
            item->setDate(recvDate);
            item->setQuantity(quantity.toInt());
            item->setTotal(total);
            break;
        }
    }
}



