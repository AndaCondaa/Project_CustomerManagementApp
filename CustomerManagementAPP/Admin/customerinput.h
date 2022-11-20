/*
 *  Program Name    :  Admin
 *  File Name       :  customerinput.h
 *  Description     :  Widget for Inputting customer
*/

#ifndef CUSTOMERINPUT_H
#define CUSTOMERINPUT_H

#include <QWidget>
#include <QSqlQueryModel>

class QLabel;
class QLineEdit;
class QPushButton;

class CustomerInput : public QWidget
{
    Q_OBJECT
public:
    explicit CustomerInput(QWidget *parent = nullptr);

signals:
    void inputCustomer();       // Signal for notifying CustomerManager that should update TableView

private slots:
    void recvCurrentCK(int);    // Receive current customerKey for setting new CustomerKey
    void input();               // Slot connected to Clicked() of InputButton
    void clear();               // Slot connected to Clicked() of ClearButton

private:
    int makeCustomerKey();      // Making new CustomerKey by combining license_number & time & index

    // Variables for setting GUI
    QLabel *ckLabel;
    QLabel *clinicLabel;
    QLabel *licenseLabel;
    QLabel *dentistLabel;
    QLabel *numberLabel;

    QLineEdit *ckLine;
    QLineEdit *clinicLine;
    QLineEdit *licenseLine;
    QLineEdit *dentistLine;
    QLineEdit *numberLine;

    QPushButton *clearButton;
    QPushButton *inputButton;

    int index;
};

#endif // CUSTOMERINPUT_H
