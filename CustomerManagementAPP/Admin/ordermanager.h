/*
 *  Program Name    :  Admin
 *  File Name       :  ordermanager.h
 *  Description     :  Management for order
*/

#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include <QWidget>

class OrderInput;
class QSqlQueryModel;
class QStandardItemModel;

namespace Ui {
class OrderManager;
}

class OrderManager : public QWidget
{
    Q_OBJECT

public:
    explicit OrderManager(QWidget *parent = nullptr);
    ~OrderManager();

    void updateTable();                     // Update OrderTableView

private slots:
    int makeTotal(int ,int);                // Calculate Total price
    void on_inputButton_clicked();          // SLot Connected to Clicked() of InputButton
    void update(int);                       // Updating Table when receive signal from OrderInput
    void on_totalButton_clicked();          // SLot Connected to Clicked() of TotalButton
    void on_searchButton_clicked();         // SLot Connected to Clicked() of SearchButton
    void on_searchComboBox_currentIndexChanged(int index);      // Changing InputMask
    void on_clearButton_clicked();          // SLot Connected to Clicked() of ClearButton
    void on_editButton_clicked();           // SLot Connected to Clicked() of EditButton
    void on_orderTableView_clicked(const QModelIndex &index);   // Checking Information for Edit
    void on_pkLine_textChanged(const QString &arg);             // Checking Price of selected product
    void on_quantityLine_textChanged(const QString &arg);       // Checking the number of stock

private:
    Ui::OrderManager *ui;

    OrderInput *orderInput;             // Objects of Class OrderInput for function what input

    QSqlQueryModel *orderQueryModel;    // Model for OrderTableView

    int tmp_price;                      // Price of selected product
    int tmp_stock;                      // The number of Stocks of selected product
};

#endif // ORDERMANAGER_H
