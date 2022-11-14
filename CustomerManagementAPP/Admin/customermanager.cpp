/*
 *  Program Name    :  Admin
 *  File Name       :  customermanager.cpp
 *  Description     :  고객관리 위젯
 *                      -> 등록된 고객 목록 출력
 *                      -> 신규등록 요청 수행
 *                      -> 검색 요청 결과 전송
 *                      -> 정보수정 요청 수행
 *                      -> 주문클래스로 고객정보 전송
*/
#include "customermanager.h"
#include "ui_customermanager.h"
#include "customerinput.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>

#include <QApplication>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

CustomerManager::CustomerManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomerManager)
{
    QSqlDatabase customerDB = QSqlDatabase::addDatabase("QODBC", "CustomerManager");
    customerDB.setDatabaseName("Oracle11gx64");
    customerDB.setUserName("customer_manager");
    customerDB.setPassword("cm");
    if (!customerDB.open()) {
        qDebug() << customerDB.lastError().text();
    } else {
        qDebug("Customer DB success");
    }

    ui->setupUi(this);
    customerInput = new CustomerInput;
    customerQueryModel = new QSqlQueryModel(ui->customerTableView);

    updateTable();

    connect(this, SIGNAL(sendCurrentCK(int)), customerInput, SLOT(recvCurrentCK(int)));
    connect(customerInput, SIGNAL(inputCustomer()), this, SLOT(update()));
}

CustomerManager::~CustomerManager()
{
    delete ui;
    QSqlDatabase customerDB = QSqlDatabase::database("CustomerManager");
    if(customerDB.isOpen()) {
        delete customerQueryModel;
        customerDB.commit();
        customerDB.close();
    }
}

// Show the CustomerInput Widget
void CustomerManager::on_inputButton_clicked()
{
    customerInput->show();
    int currentCK = customerQueryModel->index(customerQueryModel->rowCount() - 1, 0).data().toInt();
    emit sendCurrentCK(currentCK);
}

void CustomerManager::update()
{
    updateTable();
}

void CustomerManager::on_customerTableView_clicked(const QModelIndex &index)
{
    QString ck = customerQueryModel->data(index.siblingAtColumn(0)).toString();
    QString clinic = customerQueryModel->data(index.siblingAtColumn(1)).toString();
    QString license = customerQueryModel->data(index.siblingAtColumn(2)).toString();
    QString dentist = customerQueryModel->data(index.siblingAtColumn(3)).toString();
    QString number = customerQueryModel->data(index.siblingAtColumn(4)).toString();
    QString amount = customerQueryModel->data(index.siblingAtColumn(5)).toString();

    ui->ckEditLine->setText(ck);
    ui->clinicEditLine->setText(clinic);
    ui->licenseEditLine->setText(license);
    ui->dentistEditLine->setText(dentist);
    ui->numberEditLine->setText(number);
    ui->amountEditLine->setText(amount);
}

void CustomerManager::on_searchButton_clicked()
{
    QSqlDatabase customerDB = QSqlDatabase::database("CustomerManager");
    if (!ui->searchLine->text().length()) {
        QMessageBox nothing;
        nothing.setIcon(QMessageBox::Warning);
        nothing.setText(tr("Please input word for search"));
        nothing.setWindowTitle(tr("NOTHING INPUT for Search"));
        nothing.exec();
        return;
    }

    QString searchColumn = ui->searchComboBox->currentText();
    QString searchWord = ui->searchLine->text();

    customerQueryModel->setQuery
            (QString("SELECT * FROM sys.CUSTOMER_TABLE WHERE %1 LIKE '%%2%'")
                                        .arg(searchColumn, searchWord), customerDB);
    ui->customerTableView->horizontalHeader()->setStretchLastSection(true);
}

void CustomerManager::updateTable()
{
    QSqlDatabase customerDB = QSqlDatabase::database("CustomerManager");
    customerQueryModel->setQuery("SELECT * FROM sys.CUSTOMER_TABLE ORDER BY CUSTOMER_KEY", customerDB);
    customerQueryModel->setHeaderData(0, Qt::Horizontal, tr("CustomerKey"));
    customerQueryModel->setHeaderData(1, Qt::Horizontal, tr("Clinic"));
    customerQueryModel->setHeaderData(2, Qt::Horizontal, tr("License"));
    customerQueryModel->setHeaderData(3, Qt::Horizontal, tr("Dentist"));
    customerQueryModel->setHeaderData(4, Qt::Horizontal, tr("Number"));
    customerQueryModel->setHeaderData(5, Qt::Horizontal, tr("OrderCount"));

    ui->customerTableView->setModel(customerQueryModel);
    ui->customerTableView->horizontalHeader()->setStretchLastSection(true);
    ui->customerTableView->horizontalHeader()->setStyleSheet(
                "QHeaderView { font-size: 10pt; color: blue; }");
}

void CustomerManager::on_totalButton_clicked()
{
    updateTable();
    ui->searchComboBox->setCurrentIndex(0);
    ui->searchLine->clear();
}

void CustomerManager::on_editButton_clicked()
{
    QSqlDatabase customerDB = QSqlDatabase::database("CustomerManager");
    QSqlQuery editQuery(customerDB);
    editQuery.prepare("CALL sys.EDIT_CUSTOMER (:ck, :clinic, :license, :dentist, :number)");
    editQuery.bindValue(":ck", ui->ckEditLine->text());
    editQuery.bindValue(":clinic", ui->clinicEditLine->text());
    editQuery.bindValue(":license", ui->licenseEditLine->text());
    editQuery.bindValue(":dentist", ui->dentistEditLine->text());
    editQuery.bindValue(":number", ui->numberEditLine->text());
    bool isExec = editQuery.exec();

    if (isExec) {
        ui->ckEditLine->clear();
        ui->clinicEditLine->clear();
        ui->licenseEditLine->clear();
        ui->dentistEditLine->clear();
        ui->numberEditLine->clear();
        ui->amountEditLine->clear();
        updateTable();
        qDebug() << tr("EDIT succeed!");
    } else
        qDebug() << tr("EDTI fail!");
}

void CustomerManager::on_clearButton_clicked()
{
    ui->ckEditLine->clear();
    ui->clinicEditLine->clear();
    ui->licenseEditLine->clear();
    ui->dentistEditLine->clear();
    ui->numberEditLine->clear();
    ui->amountEditLine->clear();
}

void CustomerManager::on_searchComboBox_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        ui->searchLine->clear();
        ui->searchLine->setInputMask("000000");
        ui->searchLine->setPlaceholderText(tr("Input CustomerKey"));
        break;
    case 1:
        ui->searchLine->clear();
        ui->searchLine->setInputMask("NNNNNNNNNNNNNNNNNNNN");
        ui->searchLine->setPlaceholderText(tr("Input Clinic Name"));
        break;
    case 2:
        ui->searchLine->clear();
        ui->searchLine->setInputMask("00-0000-00");
        break;
    case 3:
        ui->searchLine->clear();
        ui->searchLine->setInputMask("NNNNNNNNNNNNNNNNNNNN");
        ui->searchLine->setPlaceholderText(tr("Input Number"));
        break;
    case 4:
        ui->searchLine->clear();
        ui->searchLine->setInputMask("000-0000-0000");
        break;
    }
}
