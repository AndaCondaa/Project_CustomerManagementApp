/*
 *  Program Name    :  Admin
 *  File Name       :  productinput.h
 *  Description     :  Widget for Inputting order

*/

#ifndef PRODUCTINPUT_H
#define PRODUCTINPUT_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;

class ProductInput : public QWidget
{
    Q_OBJECT

public:
    explicit ProductInput(QWidget *parent = nullptr);

signals:
    void inputProduct();

private slots:
    void recvCurrentPK(int);
    void input();
    void clear();               // Slot connected to Clicked() of ClearButton

private:
    int makeProductKey();

    // Variables for setting GUI
    QLabel *pk;
    QLabel *type;
    QLabel *name;
    QLabel *price;
    QLabel *stock;

    QLineEdit *pkLine;
    QComboBox *typeBox;
    QLineEdit *nameLine;
    QLineEdit *priceLine;
    QLineEdit *stockLine;

    QPushButton *clearButton;
    QPushButton *inputButton;

    int index;
};

#endif // PRODUCTINPUT_H
