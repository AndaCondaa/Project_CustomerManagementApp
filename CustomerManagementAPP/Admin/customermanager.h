/*
 *  Program Name    :  Admin
 *  File Name       :  customermanager.h
 *  Description     :  고객관리 위젯
 *                      -> 등록된 고객 목록 출력
 *                      -> 신규등록 요청 수행
 *                      -> 검색 요청 결과 전송
 *                      -> 정보수정 요청 수행
 *                      -> 주문클래스로 고객정보 전송
*/

#ifndef CUSTOMERMANAGER_H
#define CUSTOMERMANAGER_H

#include <QWidget>

class CustomerInput;
class QSqlQueryModel;

namespace Ui {
class CustomerManager;
}

class CustomerManager : public QWidget
{
    Q_OBJECT

public:
    explicit CustomerManager(QWidget *parent = nullptr);
    ~CustomerManager();

    void notifyCk();

signals:
    void sendCurrentCK(int);          //고객인풋클래스에 전송 for 인덱싱
    void sendCustomerKey(QVector<int>);       //오더매니저와 챗매니저에게 전송

private slots:
    void on_inputButton_clicked();      // Show the CustomerInput Widget
    void update();
    void on_customerTableView_clicked(const QModelIndex &index);
    void on_searchButton_clicked();
    void on_totalButton_clicked();
    void on_editButton_clicked();
    void on_clearButton_clicked();
    void on_searchComboBox_currentIndexChanged(int index);

private:
    void updateTable();

    Ui::CustomerManager *ui;

    CustomerInput *customerInput;        // Objects of Class CustomerInput for function what input

    QSqlQueryModel *customerQueryModel;
};

#endif // CUSTOMERMANAGER_H
