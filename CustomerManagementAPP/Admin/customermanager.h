/*
 *  Program Name    :  Admin
 *  File Name       :  customermanager.h
 *  Description     :  Management for customer
*/

#ifndef CUSTOMERMANAGER_H
#define CUSTOMERMANAGER_H

#include <QWidget>

class CustomerInput;
class QSqlQueryModel;

namespace Ui {
class CustomerManager;
}

class CustomerManager : public QWidget
{
    Q_OBJECT

public:
    explicit CustomerManager(QWidget *parent = nullptr);
    ~CustomerManager();

    void updateTable();                 // Updating CustomerTableView

signals:
    void sendCurrentCK(int);            // Sending Current CustomerKey to CustomerInput

private slots:
    void on_inputButton_clicked();      // Show the CustomerInput Widget
    void update();                      // Updating Table when receive signal from CustomerInput
    void on_customerTableView_clicked(const QModelIndex &index);    // Checking Information for Edit
    void on_searchButton_clicked();     // SLot Connected to Clicked() of searchButton
    void on_totalButton_clicked();      // SLot Connected to Clicked() of totalButton
    void on_editButton_clicked();       // SLot Connected to Clicked() of editButton
    void on_clearButton_clicked();      // SLot Connected to Clicked() of clearButton
    void on_searchComboBox_currentIndexChanged(int index);  // Changing InputMasks

private:
    Ui::CustomerManager *ui;

    CustomerInput *customerInput;       // Objects of Class CustomerInput for function what input

    QSqlQueryModel *customerQueryModel; // Model for CustomerTableView
};

#endif // CUSTOMERMANAGER_H
