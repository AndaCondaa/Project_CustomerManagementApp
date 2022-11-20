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

    void updateTable();

private slots:
    int makeTotal(int ,int);                // Calculate Total price

    void on_inputButton_clicked();
    void update(int);
    void on_totalButton_clicked();

    void on_searchButton_clicked();

    void on_searchComboBox_currentIndexChanged(int index);

    void on_clearButton_clicked();

    void on_editButton_clicked();

    void on_orderTableView_clicked(const QModelIndex &index);


    void on_pkLine_textChanged(const QString &arg1);

    void on_quantityLine_textChanged(const QString &arg1);

private:
    Ui::OrderManager *ui;

    OrderInput *orderInput;             // Objects of Class OrderInput for function what input

    QSqlQueryModel *orderQueryModel;

    int tmp_price;
    int tmp_stock;
};

#endif // ORDERMANAGER_H
