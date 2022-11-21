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

// Change the page of StackedWidget to CustomerManager
void Home::on_customerButton_clicked()
{
    emit gotoCutomer();
}

// Change the page of StackedWidget to ProductManager
void Home::on_productButton_clicked()
{
    emit gotoProduct();
}

// Change the page of StackedWidget to OrderManager
void Home::on_orderButton_clicked()
{
    emit gotoOrder();
}

// Change the page of StackedWidget to ChatManager

void Home::on_chatButton_clicked()
{
    emit gotoChat();
}

