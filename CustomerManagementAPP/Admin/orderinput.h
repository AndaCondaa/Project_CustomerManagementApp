/*
 *  Program Name    :  Admin
 *  File Name       :  orderinput.h
 *  Description     :  신규 주문을 입력하는 위젯
 *                      -> OrderManager위젯으로 정보를 전달하여, 새로운 주문객체 생성
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

signals:
    void inputOrder();

private slots:
    void clear();               // Slot connected to Clicked() of ClearButton
    void input();
    void getPrice(int);
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
};

#endif // ORDERINPUT_H
