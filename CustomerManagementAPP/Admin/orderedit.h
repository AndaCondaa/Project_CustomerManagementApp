/*
 *  Program Name    :  Admin
 *  File Name       :  orderedit.h
 *  Description     :  주문 정보를 수정하는 위젯
*/

#ifndef ORDEREDIT_H
#define ORDEREDIT_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QDateEdit;

class OrderEdit : public QWidget
{
    Q_OBJECT
public:
    explicit OrderEdit(QWidget *parent = nullptr);

signals:
    void sendOrderNum(QString);   // Send inputted OrderNumber to OrderManager for checking
    void resultEdit(QString, QString, QString, QString, QString);
                                  // Send inputted result for edit to OrderManager
private slots:
    // Slots for setting Customer and Product
    void recvCustomerKey(QString, bool);        // Receive CustomerKey for Edit from OrderManager
    void recvProductKey(QString, bool);         // Receive ProductKey for Edit from OrderManager

    // Slots for Edit
    void clear();       // Slot connected to Clicked() of ClearButton
    void isOrderNum();  // Slot connected to Clicked() of SearchButton and emit signal sendOrderNum()
    void recvResult(int, int, int, QString, int, int);
                        // Receive the Search Result from OrderManager
    void sendEdit();    // Slot connected to Clicked() and emit signal resultEdit()

private:
    //Variables for setting GUI
    QLabel *searchOrderNumber;
    QLineEdit *searchLine;              // OrderNumber for edit
    QPushButton *searchButton;          // PushButton for search aciton

    QLabel *orderNumber;
    QLabel *orderCustomer;
    QLabel *orderProduct;
    QLabel *date;
    QLabel *quantity;

    QLineEdit *orderNumLine;            // OrderNumber
    QComboBox *orderCkLine;             // Order Customer's CustomerKey
    QComboBox *orderPkLine;             // Order Product
    QDateEdit *orderDate;               // Order Date (Default value is today(system))
    QLineEdit *orderQuantity;           // Order Quantity

    QPushButton *clearButton;           // PushButton for clear action
    QPushButton *editButton;            // PushButton for edit action
};

#endif // ORDEREDIT_H
