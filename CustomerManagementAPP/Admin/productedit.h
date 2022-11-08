/*
 *  Program Name    :  Admin
 *  File Name       :  productedit.h
 *  Description     :  상품 정보를 수정하는 위젯
*/

#ifndef PRODUCTEDIT_H
#define PRODUCTEDIT_H

class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;

#include <QWidget>

class ProductEdit : public QWidget
{
    Q_OBJECT
public:
    explicit ProductEdit(QWidget *parent = nullptr);

private slots:
    void clear();       // Slot connected to Clicked() of ClearButton

private:
    //Variables for setting GUI
    QLabel *searchProductKey;
    QLineEdit *searchLine;              // ProductKey Line for search first
    QPushButton *searchButton;          // PushButton for search

    QLabel *pk;
    QLabel *type;
    QLabel *name;
    QLabel *price;

    QLineEdit *pkLine;
    QComboBox *typeLine;
    QLineEdit *nameLine;
    QLineEdit *priceLine;

    QPushButton *clearButton;
    QPushButton *editButton;
};

#endif // PRODUCTEDIT_H
