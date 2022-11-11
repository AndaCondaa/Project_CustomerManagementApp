/*
 *  Program Name    :  Admin
 *  File Name       :  ordermanager.cpp
 *  Description     :  주문관리 위젯
 *                      -> 등록된 주문 목록 출력
 *                      -> 신규주문 요청 수행
 *                      -> 검색 요청 결과 전송
 *                      -> 정보수정 요청 수행
*/


#include "ordermanager.h"
#include "ui_ordermanager.h"
#include "orderinput.h"

#include <QFile>
#include <QMessageBox>

OrderManager::OrderManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderManager)
{
    ui->setupUi(this);
    orderInput = new OrderInput;

}

OrderManager::~OrderManager()
{
    delete ui;
}

// Calculate Total price
int OrderManager::makeTotal(int price, int quantity)
{
    int total = price * quantity;
    return total;
}


