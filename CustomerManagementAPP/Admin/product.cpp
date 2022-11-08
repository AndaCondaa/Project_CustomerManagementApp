/*
 *  Program Name    :  Admin
 *  File Name       :  product.cpp
 *  Description     :  상품 클래스
 *                      -> '상품클래스 객체 1개 = 상품 1개'을 의미
*/


#include "product.h"

Product::Product(int pk, QString type, QString name, int price)
{
    setText(0, QString::number(pk));
    setText(1, type);
    setText(2, name);
    setText(3, QString::number(price));
}

//Getter functions
int Product::getProductKey() const
{
    return text(0).toInt();
}

QString Product::getType() const
{
    return text(1);
}

QString Product::getProductName() const
{
    return text(2);
}

int Product::getPrice() const
{
    return text(3).toInt();
}

//Setter functions
void Product::setType(QString type)
{
    setText(1, type);
}

void Product::setProductName(QString name)
{
    setText(2, name);
}

void Product::setPrice(int price)
{
    setText(3, QString::number(price));
}
