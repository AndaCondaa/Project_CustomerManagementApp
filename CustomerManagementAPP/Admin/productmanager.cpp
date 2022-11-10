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
#include <QSqlQueryModel>

ProductManager::ProductManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProductManager)
{
    ui->setupUi(this);

    productInput = new ProductInput;
    productQueryModel = new QSqlQueryModel(ui->productTableView);

    updateTable();

    connect(this, SIGNAL(sendCurrentPK(int)), productInput, SLOT(recvCurrentPK(int)));
    connect(productInput, SIGNAL(inputProduct()), this, SLOT(update()));
}

ProductManager::~ProductManager()
{
    delete ui;
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
    productQueryModel->setQuery("SELECT * FROM PRODUCT ORDER BY PRODUCT_KEY");
    productQueryModel->setHeaderData(0, Qt::Horizontal, tr("ProductKey"));
    productQueryModel->setHeaderData(1, Qt::Horizontal, tr("Type"));
    productQueryModel->setHeaderData(2, Qt::Horizontal, tr("Name"));
    productQueryModel->setHeaderData(3, Qt::Horizontal, tr("Price"));
    productQueryModel->setHeaderData(4, Qt::Horizontal, tr("the Number of stock"));

    ui->productTableView->setModel(productQueryModel);
    ui->productTableView->horizontalHeader()->setStretchLastSection(true);
    ui->productTableView->horizontalHeader()->setStyleSheet(
                "QHeaderView { font-size: 10pt; color: blue; }");
    ui->productTableView->resizeColumnsToContents();
}

void ProductManager::update()
{
    updateTable();
}
