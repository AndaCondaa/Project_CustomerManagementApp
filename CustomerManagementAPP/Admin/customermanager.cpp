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
#include "customeredit.h"

#include <QFile>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlQueryModel>

CustomerManager::CustomerManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomerManager)
{
    ui->setupUi(this);
    customerInput = new CustomerInput;
    customerEdit = new CustomerEdit;   
    customerQueryModel = new QSqlQueryModel(ui->customerTableView);

    updateTable();

    connect(this, SIGNAL(sendCurrentCK(int)), customerInput, SLOT(recvCurrentCK(int)));
    connect(customerInput, SIGNAL(inputCustomer()), this, SLOT(update()));
}

CustomerManager::~CustomerManager()
{
    delete ui;
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
    if (!ui->searchLine->text().length()) {
        QMessageBox nothing;
        nothing.setIcon(QMessageBox::Warning);
        nothing.setText(tr("Please input word for search"));
        nothing.setWindowTitle(tr("NOTHING INPUT for Search"));
        nothing.exec();
        return;
    }

    QString searchFlag = ui->searchComboBox->currentText();
    QString searchWord = ui->searchLine->text();

    customerQueryModel->setQuery
            (QString("SELECT * FROM CUSTOMER WHERE %1 LIKE '%%2%'")
                                        .arg(searchFlag, searchWord));
}

void CustomerManager::updateTable()
{
    customerQueryModel->setQuery("SELECT * FROM CUSTOMER ORDER BY CUSTOMER_KEY");
    customerQueryModel->setHeaderData(0, Qt::Horizontal, tr("CustomerKey"));
    customerQueryModel->setHeaderData(1, Qt::Horizontal, tr("Clinic"));
    customerQueryModel->setHeaderData(2, Qt::Horizontal, tr("License"));
    customerQueryModel->setHeaderData(3, Qt::Horizontal, tr("Dentist"));
    customerQueryModel->setHeaderData(4, Qt::Horizontal, tr("Number"));
    customerQueryModel->setHeaderData(5, Qt::Horizontal, tr("OrderAmount"));

    ui->customerTableView->setModel(customerQueryModel);
    ui->customerTableView->horizontalHeader()->setStretchLastSection(true);
    ui->customerTableView->horizontalHeader()->setStyleSheet(
                "QHeaderView { font-size: 10pt; color: blue; }");
    ui->customerTableView->resizeColumnsToContents();
}

void CustomerManager::on_totalButton_clicked()
{
    updateTable();
    ui->searchComboBox->setCurrentIndex(0);
    ui->searchLine->clear();
}

