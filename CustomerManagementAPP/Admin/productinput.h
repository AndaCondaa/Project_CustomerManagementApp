/*
 *  Program Name    :  Admin
 *  File Name       :  productinput.h
 *  Description     :  신규 상품을 입력하는 위젯
 *                      -> ProductManager위젯으로 정보를 전달하여, 새로운 상품객체 생성
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

private slots:
    void clear();               // Slot connected to Clicked() of ClearButton

private:
    // Variables for setting GUI
    QLabel *pk;
    QLabel *type;
    QLabel *name;
    QLabel *price;

    QLineEdit *pkLine;
    QComboBox *typeLine;
    QLineEdit *nameLine;
    QLineEdit *priceLine;

    QPushButton *clearButton;
    QPushButton *inputButton;
};

#endif // PRODUCTINPUT_H
