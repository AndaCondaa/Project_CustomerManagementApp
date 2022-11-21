/*
 *  Program Name    :  Admin
 *  File Name       :  orderinput.h
 *  Description     :  Widget for Inputting order
*/

#ifndef ORDERINPUT_H
#define ORDERINPUT_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QDateEdit;

class OrderInput : public QWidget
{
    Q_OBJECT
public:
    explicit OrderInput(QWidget *parent = nullptr);

    void fillCombo();           // Fill ComboBox for input

signals:
    void inputOrder(int);       // Send Signal to CustomerManager when suceed inputting

private slots:
    void clear();               // Slot connected to Clicked() of ClearButton
    void input();               // Slot connected to Clicked() of InputButton
    void getInfo(int);          // Get Stock Information from ProductTable
    void calTotal(QString);     // Calculate total price

private:
    QString makeOrderNumber();      // Making Unique OrderNumber

    // Variables for setting GUI
    QLabel *orderNum;
    QLabel *orderCK;
    QLabel *orderPK;
    QLabel *date;
    QLabel *quantity;
    QLabel *total;

    QLineEdit *orderNumLine;
    QComboBox *orderCkBox;
    QComboBox *orderPkBox;
    QDateEdit *dateEdit;
    QLineEdit *quantityLine;
    QLineEdit *totalLine;

    QPushButton *clearButton;
    QPushButton *inputButton;

    int tmp_price;            // Price of selected product
    int tmp_stock;            // Stocks of selected product
};

#endif // ORDERINPUT_H
