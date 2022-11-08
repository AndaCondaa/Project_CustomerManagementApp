/*
 *  Program Name    :  Admin
 *  File Name       :  customerinput.h
 *  Description     :  새로운 고객을 입력하는 위젯
 *                      -> CustomerManager위젯으로 정보를 전달하여, 새로운 고객객체 생성
*/

#ifndef CUSTOMERINPUT_H
#define CUSTOMERINPUT_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;

class CustomerInput : public QWidget
{
    Q_OBJECT
public:
    explicit CustomerInput(QWidget *parent = nullptr);

signals:
    // Send inputted result to CustomerManager for checking
    void input(QString, QString, QString, QString);

private slots:
    void clear();          // Slot connected to Clicked() of ClearButton
    void inputEmit();      // Slot connected to Clicked() of InputButton

private:
    // Variables for setting GUI
    QLabel *ck;
    QLabel *clinic;
    QLabel *license;
    QLabel *dentist;
    QLabel *number;

    QLineEdit *ckLine;
    QLineEdit *clinicLine;
    QLineEdit *licenseLine;
    QLineEdit *dentistLine;
    QLineEdit *numberLine;

    QPushButton *clearButton;
    QPushButton *inputButton;
};

#endif // CUSTOMERINPUT_H
