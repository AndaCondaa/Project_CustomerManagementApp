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

CustomerManager::CustomerManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomerManager)
{
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
    notifyCk();
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
            (QString("SELECT * FROM CUSTOMER_TABLE WHERE %1 LIKE '%%2%'")
                                        .arg(searchFlag, searchWord));
}

void CustomerManager::updateTable()
{
    customerQueryModel->setQuery("SELECT * FROM CUSTOMER_TABLE ORDER BY CUSTOMER_KEY");
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

void CustomerManager::on_editButton_clicked()
{
    QSqlQuery editQuery;
    editQuery.prepare("CALL EDIT_CUSTOMER (:ck, :clinic, :license, :dentist, :number)");
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

void CustomerManager::notifyCk()
{
    //업데이트하면 오더와 챗에 ck 전송
    QSqlQuery sendQuery;
    sendQuery.exec("SELECT * FROM CUSTOMER_TABLE ORDER BY CUSTOMER_KEY");

    QVector<int> ckVector;
    while (sendQuery.next()) {
        ckVector.append(sendQuery.value(0).toInt());
    }
    emit sendCustomerKey(ckVector);
}
