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

signals:
    void sendCK(QString);       // Send inputted CustomerKey to CustomerManager for checking
    void resultEdit(QString, QString, QString, QString, QString);
                                // Send inputted result for edit to CustomerManager
private slots:
    void clear();               // Slot connected to Clicked() of ClearButton
    void isCK();                // Slot connected to Clicked() of SearchButton and emit signal sendCK()
    void recvSearchForEdit(QString, QString, QString, QString, QString);
                                // Receive the Search Result from CutomerManager
    void sendEdit();            // Slot connected to Clicked() and emit signal resultEdit()

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
