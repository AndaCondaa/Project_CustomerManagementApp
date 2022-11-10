/*
 *  Program Name    :  Admin
 *  File Name       :  ordermanager.h
 *  Description     :  주문관리 위젯
 *                      -> 등록된 주문 목록 출력
 *                      -> 신규주문 요청 수행
 *                      -> 검색 요청 결과 전송
 *                      -> 정보수정 요청 수행
*/

#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include <QWidget>

class OrderInput;

namespace Ui {
class OrderManager;
}

class OrderManager : public QWidget
{
    Q_OBJECT

public:
    explicit OrderManager(QWidget *parent = nullptr);
    ~OrderManager();

private slots:
    int makeTotal(int ,int);                // Calculate Total price

    void on_inputButton_clicked();          // Show the OrderInput Widget

private:
    Ui::OrderManager *ui;

    OrderInput *orderInput;             // Objects of Class OrderInput for function what input
};

#endif // ORDERMANAGER_H
