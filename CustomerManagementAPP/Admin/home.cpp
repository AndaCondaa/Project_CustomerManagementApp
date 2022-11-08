/*
 *  Program Name    :  Admin
 *  File Name       :  home.cpp
 *  Description     :  프로그램의 메인을 구성하는 위젯
 *                      -> 고객, 상품, 주문, 채팅 매니저의 네 가지 세션으로 이동할 수 있는 버튼으로 구성
*/


#include "home.h"
#include "ui_home.h"

Home::Home(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home)
{
    //Set GUI
    ui->setupUi(this);
}

Home::~Home()
{
    delete ui;
}

void Home::on_customerButton_clicked()
{
    emit gotoCutomer();
}

void Home::on_productButton_clicked()
{
    emit gotoProduct();
}

void Home::on_orderButton_clicked()
{
    emit gotoOrder();
}

void Home::on_chatButton_clicked()
{
    emit gotoChat();
}

