/*
 *  Program Name    :  Admin
 *  File Name       :  customer.h
 *  Description     :  고객 클래스
 *                      -> '고객클래스 객체 1개 = 고객 1명'을 의미
*/

#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <QTreeWidgetItem>

class Customer : public QTreeWidgetItem
{
public:
    explicit Customer(int ck = 0, QString = "", QString = "",
                      QString = "", QString = "");

    //Getter functions
    int getCustomerKey() const;
    QString getClinic() const;
    QString getLicense() const;
    QString getDentist() const;
    QString getNumber() const;

    //Setter functions
    void setClinic(QString);
    void setLicense(QString);
    void setDentist(QString);
    void setNumber(QString);
};

#endif // CUSTOMER_H
