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

signals:
    void sendPK(QString);       // Send inputted ProductKey to ProductManager for checking
    void resultEdit(QString, QString, QString, QString);
                                // Send inputted result for edit to ProductManager
private slots:
    void clear();       // Slot connected to Clicked() of ClearButton
    void isPK();        // Slot connected to Clicked() of SearchButton and emit signal sendPk()
    void recvResult(QString, QString, QString, QString);
                        // Receive the Search Result from ProductManager
    void sendEdit();    // Slot connected to Clicked() and emit signal resultEdit()

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
