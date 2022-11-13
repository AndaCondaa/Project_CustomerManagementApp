/*
 *  Program Name    :  Admin
 *  File Name       :  productmanager.cpp
 *  Description     :  상품관리 위젯
 *                      -> 등록된 상품 목록 출력
 *                      -> 신규등록 요청 수행
 *                      -> 검색 요청 결과 전송
 *                      -> 정보수정 요청 수행
 *                      -> 주문클래스로 상품정보 전송
*/


#include "productmanager.h"
#include "ui_productmanager.h"
#include "productinput.h"

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

ProductManager::ProductManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProductManager)
{
    ui->setupUi(this);

    QSqlDatabase productDB = QSqlDatabase::addDatabase("QODBC", "ProductManager");
    productDB.setDatabaseName("Oracle11gx64");
    productDB.setUserName("PRODUCT_MANAGER");
    productDB.setPassword("pm");
    if (!productDB.open()) {
        qDebug() << productDB.lastError().text();
    } else {
        qDebug("Product DB connect success");
    }

    productInput = new ProductInput;
    productQueryModel = new QSqlQueryModel(ui->productTableView);

    updateTable();

    connect(this, SIGNAL(sendCurrentPK(int)), productInput, SLOT(recvCurrentPK(int)));
    connect(productInput, SIGNAL(inputProduct()), this, SLOT(update()));

    // 타입콤보박스 세팅 -> 타입테이블에서 가져오기
    QSqlQuery query(productDB);
    query.exec("SELECT * FROM sys.PRODUCT_TYPE ORDER BY TYPE_ID");
    while (query.next())
         ui->typeBox->addItem(query.value(1).toString());
}

ProductManager::~ProductManager()
{
    delete ui;
    QSqlDatabase productDB = QSqlDatabase::database("ProductManager");
    if(productDB.isOpen()) {
        delete productQueryModel;
        productDB.commit();
        productDB.close();
    }
}

// Show the ProductInput Widget
void ProductManager::on_inputButton_clicked()
{
    productInput->show();
    int currentPK = productQueryModel->index(productQueryModel->rowCount() - 1, 0).data().toInt();
    emit sendCurrentPK(currentPK);
}

void ProductManager::updateTable()
{
    QSqlDatabase productDB = QSqlDatabase::database("ProductManager");
    productQueryModel->setQuery("SELECT * FROM sys.PRODUCT_TABLE ORDER BY PRODUCT_KEY", productDB);
    productQueryModel->setHeaderData(0, Qt::Horizontal, tr("ProductKey"));
    productQueryModel->setHeaderData(1, Qt::Horizontal, tr("Type ID"));
    productQueryModel->setHeaderData(2, Qt::Horizontal, tr("Name"));
    productQueryModel->setHeaderData(3, Qt::Horizontal, tr("Price"));
    productQueryModel->setHeaderData(4, Qt::Horizontal, tr("the Number of stock"));

    ui->productTableView->setModel(productQueryModel);
    ui->productTableView->horizontalHeader()->setStretchLastSection(true);
    ui->productTableView->horizontalHeader()->setStyleSheet(
                "QHeaderView { font-size: 10pt; color: blue; }");
}

void ProductManager::update()
{
    updateTable();
}

void ProductManager::on_totalButton_clicked()
{
    updateTable();
    ui->searchComboBox->setCurrentIndex(0);
    ui->searchLine->clear();
}

void ProductManager::on_searchComboBox_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        ui->searchLine->clear();
        ui->searchLine->setInputMask("0000");
        ui->searchLine->setPlaceholderText(tr("Input ProductKey"));
        break;
    case 1:
        ui->searchLine->clear();
        ui->searchLine->setInputMask("00");
        ui->searchLine->setPlaceholderText(tr("Input Type ID"));
        break;
    case 2:
        ui->searchLine->clear();
        ui->searchLine->setInputMask("NNNNNNNNNN");
        ui->searchLine->setPlaceholderText(tr("Input Product Name"));
        break;
    case 3:
        ui->searchLine->clear();
        ui->searchLine->setInputMask("000000");
        ui->searchLine->setPlaceholderText(tr("Input Product Price"));
        break;
    }
}

void ProductManager::on_productTableView_clicked(const QModelIndex &index)
{
    QString pk = productQueryModel->data(index.siblingAtColumn(0)).toString();
    int typeID = productQueryModel->data(index.siblingAtColumn(1)).toInt();
    QString name = productQueryModel->data(index.siblingAtColumn(2)).toString();
    QString price = productQueryModel->data(index.siblingAtColumn(3)).toString();
    QString stock = productQueryModel->data(index.siblingAtColumn(4)).toString();

    ui->pkEditLine->setText(pk);
    ui->typeBox->setCurrentIndex(typeID-1);
    ui->nameEditLine->setText(name);
    ui->priceEditLine->setText(price);
    ui->stockEditLine->setText(stock);
}

void ProductManager::on_searchButton_clicked()
{
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

    QSqlDatabase productDB = QSqlDatabase::database("ProductManager");
    productQueryModel->setQuery
            (QString("SELECT * FROM sys.PRODUCT_TABLE WHERE %1 LIKE '%%2%'")
                                        .arg(searchColumn, searchWord), productDB);
    ui->productTableView->horizontalHeader()->setStretchLastSection(true);

}

void ProductManager::on_clearButton_clicked()
{
    ui->pkEditLine->clear();
    ui->typeBox->setCurrentIndex(0);
    ui->nameEditLine->clear();
    ui->priceEditLine->clear();
    ui->stockEditLine->clear();
}

void ProductManager::on_editButton_clicked()
{
    QSqlDatabase productDB = QSqlDatabase::database("ProductManager");
    QSqlQuery editQuery(productDB);
    editQuery.prepare("CALL sys.EDIT_PRODUCT (:pk, :type_id, :name, :price, :stock)");
    editQuery.bindValue(":pk", ui->pkEditLine->text());
    editQuery.bindValue(":type_id", ui->typeBox->currentIndex() + 1);
    editQuery.bindValue(":name", ui->nameEditLine->text());
    editQuery.bindValue(":price", ui->priceEditLine->text());
    editQuery.bindValue(":stock", ui->stockEditLine->text());
    bool isExec = editQuery.exec();

    if (isExec) {
        ui->pkEditLine->clear();
        ui->typeBox->setCurrentIndex(0);
        ui->nameEditLine->clear();
        ui->priceEditLine->clear();
        ui->stockEditLine->clear();
        updateTable();
        qDebug() << tr("EDIT succeed!");
    } else
        qDebug() << tr("EDTI fail!");
}

void ProductManager::recvPk(QString pk)
{
    QModelIndexList index = productQueryModel->match(productQueryModel->index(0, 0), Qt::EditRole, pk, 1, Qt::MatchFlag(Qt::MatchCaseSensitive));
    foreach (auto idx, index) {
        QString price = productQueryModel->data(idx.siblingAtColumn(3)).toString();
        QStringList result;
        result << pk << price;

        emit sendResultPrice(result);
    }
}
