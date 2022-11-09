/*
 *  Program Name    :  Admin
 *  File Name       :  customerinput.h
 *  Description     :  새로운 고객을 입력하는 위젯
 *                      -> CustomerManager위젯으로 정보를 전달하여, 새로운 고객객체 생성
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
    void inputCustomer();


private slots:
    void recvCurrentCK(int);
    void input();
    void clear();          // Slot connected to Clicked() of ClearButton

private:
    int makeCustomerKey(QString license, QString number);

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
