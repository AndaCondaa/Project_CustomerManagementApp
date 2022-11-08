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

private slots:
    // Slots for Edit
    void clear();       // Slot connected to Clicked() of ClearButton

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
