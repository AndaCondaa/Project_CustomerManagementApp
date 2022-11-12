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
class QSqlQueryModel;
class QStandardItemModel;

namespace Ui {
class OrderManager;
}

class OrderManager : public QWidget
{
    Q_OBJECT

public:
    explicit OrderManager(QWidget *parent = nullptr);
    ~OrderManager();

signals:
    void sendCkToInput(QVector<int>);       //인풋으로 고객매니저에서 받은 ck전송
    void sendPkToInput(QVector<int>);       //인풋으로 상품매니저에서 받은 pk전송
    void checkPrice(QString);               //productmanager로 가격받을 pk 보내기
    void sendResultPrice(QStringList);   //인풋으로 가격모델 보내기

private slots:
    void recvCustomerKey(QVector<int>);     //고객매니저에서 ck받기
    void recvProductKey(QVector<int>);      //상품매니저에게 pk받기
    int makeTotal(int ,int);                // Calculate Total price
    void recvPkFromInput(QString);          //인풋에서 체크할 pk 받기

    void on_inputButton_clicked();
    void update();
    void recvResultPrice(QStringList);

    void on_totalButton_clicked();

    void on_searchButton_clicked();

    void on_searchComboBox_currentIndexChanged(int index);

    void on_clearButton_clicked();

    void on_editButton_clicked();

    void on_orderTableView_clicked(const QModelIndex &index);

    void on_pkBox_currentIndexChanged(int index);

    void on_quantityLine_textChanged(const QString &arg1);

private:
    void updateTable();

    Ui::OrderManager *ui;

    OrderInput *orderInput;             // Objects of Class OrderInput for function what input

    QSqlQueryModel *orderQueryModel;
    QStandardItemModel *priceModel;
};

#endif // ORDERMANAGER_H
