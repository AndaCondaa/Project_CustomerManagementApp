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
class QStandardItemModel;

class OrderInput : public QWidget
{
    Q_OBJECT
public:
    explicit OrderInput(QWidget *parent = nullptr);

    void fillCombo();

signals:
    void inputOrder(int);

private slots:
    void clear();               // Slot connected to Clicked() of ClearButton
    void input();
    void getInfo(int);
    void calTotal(QString);

private:
    QString makeOrderNumber();

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

    int tmp_price;
    int tmp_stock;
};

#endif // ORDERINPUT_H
