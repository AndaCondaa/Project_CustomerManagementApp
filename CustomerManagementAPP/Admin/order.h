/*
 *  Program Name    :  Admin
 *  File Name       :  order.h
 *  Description     :  주문 클래스
 *                      -> '주문클래스 객체 1개 = 주문 1건'을 의미
*/

#ifndef ORDER_H
#define ORDER_H

#include <QTreeWidgetItem>

class Order : public QTreeWidgetItem
{
public:
    explicit Order(int orderNum, int orderCK, int orderPK,
                   QString date, int quantity, int total);

    //Getter functions
    int getOrderNum() const;
    int	getOrderCK() const;
    int getOrderPK() const;
    QString getDate() const;
    int getQuantity() const;
    int getTotal() const;

    //Setter functions
    void setOrderNum(int);
    void setOrderCK(int);
    void setOrderPK(int);
    void setDate(QString);
    void setQuantity(int);
    void setTotal(int);
};

#endif // ORDER_H
