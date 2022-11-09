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
#include <QMap>


class CustomerInput;
class CustomerSearch;
class CustomerEdit;
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

signals:
    void sendCurrentCK(int);

private slots:
    void on_inputButton_clicked();      // Show the CustomerInput Widget
    void update();

    void on_customerTableView_clicked(const QModelIndex &index);

    void on_searchButton_clicked();

    void on_pushButton_clicked();

private:
    void updateTable();

    Ui::CustomerManager *ui;

    CustomerInput *customerInput;        // Objects of Class CustomerInput for function what input
    CustomerSearch *customerSearch;      // Objects of Class CustomerSearch for function what search
    CustomerEdit *customerEdit;          // Objects of Class CustomerEdit for function what edit

    QSqlQueryModel *customerQueryModel;
};

#endif // CUSTOMERMANAGER_H
