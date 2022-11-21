/*
 *  Program Name    :  Admin
 *  File Name       :  productmanager.h
 *  Description     :  Management for product
*/

#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H

#include <QWidget>

class ProductInput;
class QSqlQueryModel;

namespace Ui {
class ProductManager;
}

class ProductManager : public QWidget
{
    Q_OBJECT

public:
    explicit ProductManager(QWidget *parent = nullptr);
    ~ProductManager();

    void updateTable();                     // Updating ProductTableView

signals:
    void sendCurrentPK(int);                // Sending Current ProductKey to ProductInpute
    void sendResultPrice(QStringList);      // Sending Price to OrderManager

private slots:
    void on_inputButton_clicked();          // Show the ProductInput Widget
    void update();                          // Updating Table when receive signal from ProductInput
    void on_totalButton_clicked();          // SLot Connected to Clicked() of TotalButton
    void on_searchComboBox_currentIndexChanged(int index);          // Changing InputMasks
    void on_productTableView_clicked(const QModelIndex &index);     // Checking Information for Edit
    void on_searchButton_clicked();         // SLot Connected to Clicked() of SearchButton
    void on_clearButton_clicked();          // SLot Connected to Clicked() of ClearButton
    void on_editButton_clicked();           // SLot Connected to Clicked() of EditButton
    void recvPk(QString);                   // Receive ProductKey from Order for checking price

private:

    Ui::ProductManager *ui;

    ProductInput *productInput;             // Objects of Class ProductInput for function what input

    QSqlQueryModel *productQueryModel;      // Model for ProductTableView

    QVector<int> stockOutVector;            // Products' row number List in Model
};

#endif // PRODUCTMANAGER_H
