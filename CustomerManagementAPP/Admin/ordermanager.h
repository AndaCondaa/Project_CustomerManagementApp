/*
 *  Program Name    :  Admin
 *  File Name       :  ordermanager.h
 *  Description     :  주문관리 위젯
 *                      -> 등록된 주문 목록 출력
 *                      -> 신규주문 요청 수행
 *                      -> 검색 요청 결과 전송
 *                      -> 정보수정 요청 수행
*/

#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include <QWidget>

class Order;
class OrderInput;
class OrderSearch;
class OrderEdit;
class QTreeWidgetItem;

namespace Ui {
class OrderManager;
}

class OrderManager : public QWidget
{
    Q_OBJECT

public:
    explicit OrderManager(QWidget *parent = nullptr);
    ~OrderManager();

signals:
    void sendCustomerKey(QString, bool);    // Send CustomerKey to OrderInput and OrderEdit
    void sendProductKey(QString, bool);     // Send ProductKey to OrderInput and OrderEdit

    // Send result of search to OrderSearch and OrderEdit
    void searchResult(int, int, int, QString, int, int);

private slots:
    int makeIndex();                        // Make Index for Key of Map(OrderList)
    int makeOrderNum(int, int, int);        // Make OrderNumber by using Index, CustomerKey, ProductKey
    int makeTotal(int ,int);                // Calculate Total price


    void on_inputButton_clicked();          // Show the OrderInput Widget
    void on_searchButton_clicked();         // Show the OrderSearch Widget
    void on_editButton_clicked();           // Show the OrderEdit Widget
    void on_orderTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
                                            // Slot for Remove Item (Order)

    // Receive Keys from CustomerManager and ProductManager for filling ComboBox
    void recvCustomerKey(QString, QString, bool);
    void recvProductKey(QString, QString, bool);
    void recvChangePrice(QString, QString);

    // Receive the data about new Order's information from OrderInput
    void inputData(QString, QString, QString, QString);     // Add new Order

    // Slots for Search
    void checkSearch(int, QString);     // Receive OrderNumber from OrderSearch for search
    void checkSearch(int);              // Check OrderNumber and emit signal searchResult()

    // Slots for Edit
    void recvOrderNumber(QString);      // Receive the OrderNumber from OrderEdit for check
    void recvEditResult(QString, QString, QString, QString, QString);
                                        // Receive the result of edit from OrderEdit
private:
    Ui::OrderManager *ui;

    QMap<int, Order*> orderList;        // <Index, Order>

    OrderInput *orderInput;             // Objects of Class OrderInput for function what input
    OrderSearch *orderSearch;           // Objects of Class OrderSearch for function what input
    OrderEdit *orderEdit;               // Objects of Class OrderEdit for function what input

    QHash<QString, QString> productPriceHash;   // <ProductKey, Price>
    QList<Order*> searchList;                   // list for sending SearchList for a time
};

#endif // ORDERMANAGER_H
