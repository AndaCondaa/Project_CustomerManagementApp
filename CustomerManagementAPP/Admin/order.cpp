/*
 *  Program Name    :  Admin
 *  File Name       :  order.cpp
 *  Description     :  주문 클래스
 *                      -> '주문클래스 객체 1개 = 주문 1건'을 의미
*/


#include "order.h"

Order::Order(int orderNum, int orderCK, int orderPK,
             QString date, int quantity, int total)
{
    setText(0, QString::number(orderNum));
    setText(1, QString::number(orderCK));
    setText(2, QString::number(orderPK));
    setText(3, date);
    setText(4, QString::number(quantity));
    setText(5, QString::number(total));
}

//Getter functions
int Order::getOrderNum() const
{
    return text(0).toInt();
}

int	Order::getOrderCK() const
{
    return text(1).toInt();
}

int Order::getOrderPK() const
{
    return text(2).toInt();
}

QString Order::getDate() const
{
    return text(3);
}

int Order::getQuantity() const
{
    return text(4).toInt();
}

int Order::getTotal() const
{
    return text(5).toInt();
}

//Setter functions
void Order::setOrderNum(int orderNum)
{
    setText(0, QString::number(orderNum));
}

void Order::setOrderCK(int orderCk)
{
    setText(1, QString::number(orderCk));
}

void Order::setOrderPK(int orderPK)
{
    setText(2, QString::number(orderPK));
}

void Order::setDate(QString date)
{
    setText(3, date);
}

void Order::setQuantity(int quantity)
{
    setText(4, QString::number(quantity));
}

void Order::setTotal(int total)
{
    setText(5, QString::number(total));
}
