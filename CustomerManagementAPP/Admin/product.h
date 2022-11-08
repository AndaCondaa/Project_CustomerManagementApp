/*
 *  Program Name    :  Admin
 *  File Name       :  product.h
 *  Description     :  상품 클래스
 *                      -> '상품클래스 객체 1개 = 상품 1개'을 의미
*/

#ifndef PRODUCT_H
#define PRODUCT_H

#include <QTreeWidgetItem>

class Product : public QTreeWidgetItem
{
public:
    explicit Product(int, QString, QString, int);

    //Getter functions
    int getProductKey() const;
    QString getType() const;
    QString getProductName() const;
    int getPrice() const;

    //Setter functions
    void setType(QString);
    void setProductName(QString);
    void setPrice(int);
};

#endif // PRODUCT_H
