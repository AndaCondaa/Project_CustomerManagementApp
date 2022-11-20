/*
 *  Program Name    :  Admin
 *  File Name       :  mainwindow.h
 *  Description     :  Mainwindow of Program
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Home;
class CustomerManager;
class ProductManager;
class OrderManager;
class ChatManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slot connected triggered() of Toolbar Action
    void on_action_Home_triggered();
    void on_action_Customer_triggered();
    void on_action_Product_triggered();
    void on_action_Order_triggered();
    void on_action_Chat_triggered();
    void on_action_Quit_triggered();

private:
    Ui::MainWindow* ui;

    Home* home;
    CustomerManager* customerManager;
    ProductManager* productManager;
    OrderManager* orderManager;
    ChatManager* chatManager;
};
#endif // MAINWINDOW_H
