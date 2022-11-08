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
#include "customer.h"
#include "customerinput.h"
#include "customersearch.h"
#include "customeredit.h"

#include <QFile>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

CustomerManager::CustomerManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomerManager)
{
    ui->setupUi(this);
    customerInput = new CustomerInput;
    customerSearch = new CustomerSearch;
    customerEdit = new CustomerEdit;

    // Connecting Signal and Slot for Input
    connect(customerInput, SIGNAL(input(QString,QString,QString,QString)),
            SLOT(inputData(QString,QString,QString,QString)));

    // Connecting Signal and Slot for Search
    connect(customerSearch, SIGNAL(search(int, QString)), SLOT(checkSearch(int, QString)));
    connect(this, SIGNAL(searchResult(QString, QString, QString, QString, QString)),
            customerSearch, SLOT(recvSearchResult(QString, QString, QString, QString, QString)));

    // Connecting Signal and Slot for Edit
    connect(customerEdit, SIGNAL(sendCK(QString)), SLOT(recvCK(QString)));
    connect(this, SIGNAL(searchResult(QString, QString, QString, QString, QString)),
            customerEdit, SLOT(recvSearchForEdit(QString, QString, QString, QString, QString)));
    connect(customerEdit, SIGNAL(resultEdit(QString, QString, QString, QString, QString)),
            SLOT(recvEditResult(QString, QString, QString, QString, QString)));
}

CustomerManager::~CustomerManager()
{
    //Data Saving
    QFile file("../Admin/data/manage/customer_list.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : customerList) {
        Customer* c = v;
        out << c->getCustomerKey() << ", ";
        out << c->getClinic() << ", ";
        out << c->getLicense() << ", ";
        out << c->getDentist() << ", ";
        out << c->getNumber() << "\n";
    }
    file.close( );

    delete ui;
}

// Load registerd Customer List
void CustomerManager::loadData()
{
    //Data Loading
    QFile file("../Admin/data/manage/customer_list.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size()) {
            int index = makeIndex();
            Customer* customer = new Customer(row[0].toInt(), row[1],
                    row[2], row[3], row[4]);
            ui->customerTreeWidget->addTopLevelItem(customer);
            customerList.insert(index, customer);

            //Send CustomerKey to OrderManager and ChatManager
            emit sendCustomerKey(row[0], row[1], true);
        }
    }
    file.close( );
}

// Make Index for Key of the QMap(CustomerList)
int CustomerManager::makeIndex()
{
    if (customerList.size() == 0) {
        return 1;
    }
    else {
        auto c = customerList.end();
        c--;
        int index = (c.value()->getCustomerKey()) / 10000;
        return ++index;
    }
}

// Make CustomerKey by using index, license, number
int CustomerManager::makeCustomerKey(int index, QString license, QString number) const
{
    QString tmp_license = license.split("-")[0] +
            license.split("-")[1] + license.split("-")[2];
    QString tmp_number = number.split("-")[0] +
            number.split("-")[1] + number.split("-")[2];

    // string to integer
    int temp1 = (tmp_license.toULongLong() * 111) % 10000;
    int temp2 = (tmp_number.toULongLong() * 111) % 10000;

    // making CustomerKey
    int CK = (index * 10000) + (temp1 + temp2) % 10000;     // 만의 자리 값이 고객등록 순서를 의미
    return CK;
}

// Show the CustomerInput Widget
void CustomerManager::on_inputButton_clicked()
{
    customerInput->show();
}

// Show the CustomerSearch Widget
void CustomerManager::on_searchButton_clicked()
{
    customerSearch->show();
}

// Show the CustomerEdit Widget
void CustomerManager::on_editButton_clicked()
{
    customerEdit->show();
}

// Slot for Remove Item (Customer)
void CustomerManager::on_customerTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    QString ck = item->text(0);
    QString clinic = item->text(1);
    QString dentist = item->text(3);

    // Check that user remove the customer really
    QMessageBox remove;
    remove.setIcon(QMessageBox::Warning);
    remove.setText(ck + " " + clinic + " " + dentist + " REMOVE?");
    remove.addButton(tr("YES"), QMessageBox::ActionRole);
    remove.addButton(tr("NO"), QMessageBox::ActionRole);
    remove.exec();

    // Remove
    if (remove.clickedButton()->text() == "YES") {
        foreach (auto removeItem, customerList) {
            if (removeItem->getCustomerKey() == ck.toInt()) {
                //Send CustomerKey to OrderManager and ChatManager
                emit sendCustomerKey(removeItem->text(0), removeItem->text(1), false);
                customerList.remove(customerList.key(removeItem));
            }
        ui->customerTreeWidget->takeTopLevelItem(ui->customerTreeWidget->indexOfTopLevelItem(item));
        ui->customerTreeWidget->update();
        }
    }
}

// Add new Customer
void CustomerManager::inputData
        (QString clinic, QString license, QString dentist,
         QString number)
{
    int index = makeIndex();
    int ck = makeCustomerKey(index, license, number);

    Customer* customer = new Customer
            (ck, clinic, license, dentist, number);

    ui->customerTreeWidget->addTopLevelItem(customer);
    customerList.insert(index, customer);
    // Send CustomerKey to OrderManager and ChatManager
    emit sendCustomerKey(QString::number(ck), clinic, true);    //true: 고객추가
}

// Receive CusomerKey from CustomerSearch for search
void CustomerManager::checkSearch(int index, QString input)
{
    int op = 0;
    searchList.clear();
    switch (index) {
    case 0: //CustomerKey
        for(auto c = customerList.begin() ; c != customerList.end(); c++) {
            if (QString::number(c.value()->getCustomerKey()).contains(input)) {
                checkSearch(c.key());
                op++;
            }
        }
        break;
    case 1: //Clinic Name
        for(auto c = customerList.begin() ; c != customerList.end(); c++) {
            if (c.value()->getClinic().contains(input, Qt::CaseInsensitive)) {
                checkSearch(c.key());
                op++;
            }
        }
        break;
    case 2: //License
        for(auto c = customerList.begin() ; c != customerList.end(); c++) {
            if (c.value()->getLicense().contains(input, Qt::CaseInsensitive)) {
                checkSearch(c.key());
                op++;
            }
        }
        break;
    case 3: //Dentist Name
        for(auto c = customerList.begin() ; c != customerList.end(); c++) {
            if (c.value()->getDentist().contains(input, Qt::CaseInsensitive)) {
                checkSearch(c.key());
                op++;
            }
        }
        break;
    case 4: //Number
        for(auto c = customerList.begin() ; c != customerList.end(); c++) {
            if (c.value()->getNumber().contains(input)) {
                checkSearch(c.key());
                op++;
            }
        }
        break;
    }

    // If there is no result, show the MessageBox
    if (op == 0) {
        QMessageBox nothing;
        nothing.button(QMessageBox::Ok);
        nothing.setText("No Result");
        nothing.setWindowTitle("No Result");
        nothing.exec();
    }
}

// Check CustomerKey and emit signal searchResult()
void CustomerManager::checkSearch(int key)
{
    Customer* customer = customerList.value(key);
    // Send search result to CustomerSearch and CustomerEdit
    emit searchResult(QString::number(customer->getCustomerKey()), customer->getClinic(),
                      customer->getDentist(), customer->getLicense(),
                      customer->getNumber());
}

// Receive CusomerKey from CustomerSearch for Edit
void CustomerManager::recvCK(QString ck)
{
    foreach (auto item, ui->customerTreeWidget->findItems(ck, Qt::MatchExactly, 0)) {
        // Send search result to CustomerSearch and CustomerEdit
        emit searchResult(item->text(0), item->text(1),
                            item->text(2), item->text(3), item->text(4));
    }
    if ((ui->customerTreeWidget->findItems(ck, Qt::MatchExactly, 0)).isEmpty()) {
        QMessageBox nothing;
        nothing.button(QMessageBox::Ok);
        nothing.setText("No Result");
        nothing.setWindowTitle("No Result");
        nothing.exec();
    }
}

// Receive result information for edit from CustomerEdit
void CustomerManager::recvEditResult
(QString ck, QString clinic, QString license, QString dentist, QString number)
{
    foreach (auto item, customerList.values()) {
        if (item->getCustomerKey() == ck.toInt()) {
            item->setClinic(clinic);
            item->setLicense(license);
            item->setDentist(dentist);
            item->setNumber(number);
            break;
        }
    }
}





