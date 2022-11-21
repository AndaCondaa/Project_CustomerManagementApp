#include "productmanager.h"
#include "ui_productmanager.h"
#include "productinput.h"
#include "productdelegate.h"

#include <QMessageBox>
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
    productDB.setUserName("product_manager");
    productDB.setPassword("pm");
    if (!productDB.open()) {
        qDebug() << productDB.lastError().text();
    } else {
        qDebug("Product DB connect success");
    }

    productInput = new ProductInput;
    productQueryModel = new QSqlQueryModel(ui->productTableView);

    connect(this, SIGNAL(sendCurrentPK(int)), productInput, SLOT(recvCurrentPK(int)));
    connect(productInput, SIGNAL(inputProduct()), this, SLOT(update()));

    // Setting the TypeComboBox by getting information from product_type(table)
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

// Updating ProductTableView
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

    stockOutVector.clear();

    QModelIndexList indexes = productQueryModel->match(productQueryModel->index(0, 4), Qt::EditRole, 0, -1, Qt::MatchFlags(Qt::MatchExactly));
    foreach (auto idx, indexes) {
        stockOutVector.append(idx.row());
    }

    ProductDelegate *delegate = new ProductDelegate(ui->productTableView);
    delegate->setOutVector(stockOutVector);
    ui->productTableView->setItemDelegateForColumn(4, delegate);
}

// Show the ProductInput Widget
void ProductManager::on_inputButton_clicked()
{
    productInput->show();
    int currentPK = productQueryModel->index(productQueryModel->rowCount() - 1, 0).data().toInt();
    emit sendCurrentPK(currentPK);
}

// Updating Table when receive signal from ProductInput
void ProductManager::update()
{
    updateTable();
}

// SLot Connected to Clicked() of TotalButton
void ProductManager::on_totalButton_clicked()
{
    updateTable();
    ui->searchComboBox->setCurrentIndex(0);
    ui->searchLine->clear();
}

// Changing InputMasks
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

// Checking Information for Edit
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

// SLot Connected to Clicked() of SearchButton
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

// SLot Connected to Clicked() of ClearButton
void ProductManager::on_clearButton_clicked()
{
    ui->pkEditLine->clear();
    ui->typeBox->setCurrentIndex(0);
    ui->nameEditLine->clear();
    ui->priceEditLine->clear();
    ui->stockEditLine->clear();
}

// SLot Connected to Clicked() of EditButton
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

// Receive ProductKey from Order for checking price
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
