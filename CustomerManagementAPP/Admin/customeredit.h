/*
 *  Program Name    :  Admin
 *  File Name       :  customeredit.h
 *  Description     :  고객 정보를 수정하는 위젯
*/

#ifndef CUSTOMEREDIT_H
#define CUSTOMEREDIT_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;

class CustomerEdit : public QWidget
{
    Q_OBJECT
public:
    explicit CustomerEdit(QWidget *parent = nullptr);

private slots:
    void clear();               // Slot connected to Clicked() of ClearButton

private:
    //Variables for setting GUI
    QLabel *customerKey;
    QLineEdit *searchLine;              // CustomerKey for edit
    QPushButton *searchButton;          // PushButton for search action

    QLabel *ck;
    QLabel *clinic;
    QLabel *license;
    QLabel *dentist;
    QLabel *number;

    QLineEdit *ckLine;                  // CustomerKey
    QLineEdit *clinicLine;              // Clinic Name
    QLineEdit *licenseLine;             // License Number
    QLineEdit *dentistLine;             // Dentist Name
    QLineEdit *numberLine;              // Number (Phone)

    QPushButton *clearButton;           // PushButton for clear action
    QPushButton *editButton;            // PushButton for edit action
};

#endif // CUSTOMEREDIT_H
