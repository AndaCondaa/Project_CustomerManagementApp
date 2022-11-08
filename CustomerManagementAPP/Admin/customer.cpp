/*
 *  Program Name    :  Admin
 *  File Name       :  customer.cpp
 *  Description     :  고객 클래스
 *                      -> '고객클래스 객체 1개 = 고객 1명'을 의미
*/

#include "customer.h"

Customer::Customer(int ck, QString clinic,
                   QString license, QString dentist,
                   QString number)
{
    setText(0, QString::number(ck));
    setText(1, clinic);
    setText(2, license);
    setText(3, dentist);
    setText(4, number);
}

//Getter functions
int Customer::getCustomerKey() const
{
    return text(0).toInt();
}

QString Customer::getClinic() const
{
    return text(1);
}

QString Customer::getLicense() const
{
    return text(2);
}

QString Customer::getDentist() const
{
    return text(3);
}

QString Customer::getNumber() const
{
    return text(4);
}

//Setter functions
void Customer::setClinic(QString clinic)
{
    setText(1, clinic);
}

void Customer::setLicense(QString license)
{
    setText(2, license);
}

void Customer::setDentist(QString dentist)
{
    setText(3, dentist);
}

void Customer::setNumber(QString number)
{
    setText(4, number);
}

