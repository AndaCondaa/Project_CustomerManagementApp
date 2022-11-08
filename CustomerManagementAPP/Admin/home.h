/*
 *  Program Name    :  Admin
 *  File Name       :  home.h
 *  Description     :  프로그램의 메인을 구성하는 위젯
 *                      -> 고객, 상품, 주문, 채팅 매니저의 네 가지 세션으로 이동할 수 있는 버튼으로 구성
*/

#ifndef HOME_H
#define HOME_H

#include <QWidget>
#include <QSqlQueryModel>
#include <QTableView>

namespace Ui {
class Home;
}

class Home : public QWidget
{
    Q_OBJECT

public:
    explicit Home(QWidget *parent = nullptr);
    ~Home();

signals:
    void gotoCutomer();                 // Change the page of StackedWidget to CustomerManager
    void gotoProduct();                 // Change the page of StackedWidget to ProductManager
    void gotoOrder();                   // Change the page of StackedWidget to OrderManager
    void gotoChat();                    // Change the page of StackedWidget to ChatManager

private slots:
    // Slot connected to Clicked() of Buttons
    void on_customerButton_clicked();
    void on_productButton_clicked();
    void on_orderButton_clicked();
    void on_chatButton_clicked();

private:
    Ui::Home *ui;
};

#endif // HOME_H
