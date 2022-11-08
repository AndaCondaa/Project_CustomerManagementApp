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
#include "productsearch.h"
#include "productedit.h"

#include <QFile>
#include <QMessageBox>

ProductManager::ProductManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProductManager)
{
    ui->setupUi(this);

    productInput = new ProductInput;
    productSearch = new ProductSearch;
    productEdit = new ProductEdit;
}

ProductManager::~ProductManager()
{
    delete ui;
}

// Show the ProductInput Widget
void ProductManager::on_inputButton_clicked()
{
    productInput->show();
}

// Show the ProductSearch Widget
void ProductManager::on_searchButton_clicked()
{
    productSearch->show();
}

// Show the ProductEdit Widget
void ProductManager::on_editButton_clicked()
{
    productEdit->show();
}
