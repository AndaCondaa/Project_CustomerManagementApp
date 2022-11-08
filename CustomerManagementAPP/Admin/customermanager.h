/*
 *  Program Name    :  Admin
 *  File Name       :  customermanager.h
 *  Description     :  고객관리 위젯
 *                      -> 등록된 고객 목록 출력
 *                      -> 신규등록 요청 수행
 *                      -> 검색 요청 결과 전송
 *                      -> 정보수정 요청 수행
 *                      -> 주문클래스로 고객정보 전송
*/

#ifndef CUSTOMERMANAGER_H
#define CUSTOMERMANAGER_H

#include <QWidget>
#include <QMap>

class Customer;
class CustomerInput;
class CustomerSearch;
class CustomerEdit;
class QTreeWidgetItem;
class QAbstractButton;

namespace Ui {
class CustomerManager;
}

class CustomerManager : public QWidget
{
    Q_OBJECT

public:
    explicit CustomerManager(QWidget *parent = nullptr);
    ~CustomerManager();

    void loadData();            // Load registerd Customer List

signals:
    void sendCustomerKey(QString, QString, bool);   // Send CustomerKey to OrderManager and ChatManager
    void searchResult(QString, QString, QString, QString, QString);
                                        // Send search result to CustomerSearch and CustomerEdit
private slots:
    int makeIndex();                    // Make Index for Key of Map(CustomerList)
    int makeCustomerKey(int, QString, QString) const;   // Make CustomerKey by using index, license, number

    void on_inputButton_clicked();      // Show the CustomerInput Widget
    void on_searchButton_clicked();     // Show the CustomerSearch Widget
    void on_editButton_clicked();       // Show the CustomerEdit Widget
    void on_customerTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
                                        // Slot for Remove Item (Customer)

    // Receive the data about new Customer's information from CustomerInput
    void inputData(QString, QString, QString, QString);     // Add new Customer

    // Slots for Search
    void checkSearch(int, QString);     // Receive CusomerKey from CustomerSearch for search
    void checkSearch(int);              // Check CustomerKey and emit signal searchResult()

    // Slots for Edit
    void recvCK(QString);               // Receive CusomerKey from CustomerEdit for Edit
    void recvEditResult(QString, QString, QString, QString, QString);
                                        // Receive result information for edit from CustomerEdit
private:
    Ui::CustomerManager *ui;

    QMap<int, Customer*> customerList;   // <Index, Customer>

    CustomerInput *customerInput;        // Objects of Class CustomerInput for function what input
    CustomerSearch *customerSearch;      // Objects of Class CustomerSearch for function what search
    CustomerEdit *customerEdit;          // Objects of Class CustomerEdit for function what edit

    QList<Customer*> searchList;         // list for sending SearchList for a time
};

#endif // CUSTOMERMANAGER_H
