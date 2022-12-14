#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "home.h"
#include "customermanager.h"
#include "productmanager.h"
#include "ordermanager.h"
#include "chatmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Set GUI
    ui->setupUi(this);

    home = new Home;
    customerManager = new CustomerManager;
    productManager = new ProductManager;
    orderManager = new OrderManager;
    chatManager = new ChatManager;

    ui->stackedWidget->insertWidget(0, home);
    ui->stackedWidget->insertWidget(1, customerManager);
    ui->stackedWidget->insertWidget(2, productManager);
    ui->stackedWidget->insertWidget(3, orderManager);
    ui->stackedWidget->insertWidget(4, chatManager);

    ui->stackedWidget->setCurrentIndex(0);

    // Connection Signal and Slot for Homa menu
    connect(home, SIGNAL(gotoCutomer()), SLOT(on_action_Customer_triggered()));
    connect(home, SIGNAL(gotoProduct()), SLOT(on_action_Product_triggered()));
    connect(home, SIGNAL(gotoOrder()), SLOT(on_action_Order_triggered()));
    connect(home, SIGNAL(gotoChat()), SLOT(on_action_Chat_triggered()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Slot connected triggered() of Toolbar Action
void MainWindow::on_action_Home_triggered()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->action_Home->setChecked(true);
    ui->action_Customer->setChecked(false);
    ui->action_Product->setChecked(false);
    ui->action_Order->setChecked(false);
    ui->action_Chat->setChecked(false);
}

// Slot connected triggered() of Cusotmer Action
void MainWindow::on_action_Customer_triggered()
{
    customerManager->updateTable();
    customerManager->showMaximized();
    ui->stackedWidget->setCurrentIndex(1);
    ui->action_Home->setChecked(false);
    ui->action_Customer->setChecked(true);
    ui->action_Product->setChecked(false);
    ui->action_Order->setChecked(false);
    ui->action_Chat->setChecked(false);
}

// Slot connected triggered() of Product Action
void MainWindow::on_action_Product_triggered()
{
    productManager->updateTable();
    productManager->showMaximized();
    ui->stackedWidget->setCurrentIndex(2);
    ui->action_Home->setChecked(false);
    ui->action_Customer->setChecked(false);
    ui->action_Product->setChecked(true);
    ui->action_Order->setChecked(false);
    ui->action_Chat->setChecked(false);
}

// Slot connected triggered() of Order Action
void MainWindow::on_action_Order_triggered()
{
    orderManager->updateTable();
    orderManager->showMaximized();
    ui->stackedWidget->setCurrentIndex(3);
    ui->action_Home->setChecked(false);
    ui->action_Customer->setChecked(false);
    ui->action_Product->setChecked(false);
    ui->action_Order->setChecked(true);
    ui->action_Chat->setChecked(false);
}

// Slot connected triggered() of Chat Action
void MainWindow::on_action_Chat_triggered()
{
    chatManager->updateNotice();
    chatManager->updateCustomerList();
    chatManager->updateFileList();
    chatManager->showMaximized();
    ui->stackedWidget->setCurrentIndex(4);
    ui->action_Home->setChecked(false);
    ui->action_Customer->setChecked(false);
    ui->action_Product->setChecked(false);
    ui->action_Order->setChecked(false);
    ui->action_Chat->setChecked(true);
}

// Slot connected triggered() of Quit Action
void MainWindow::on_action_Quit_triggered()
{
    qApp->quit();
}







