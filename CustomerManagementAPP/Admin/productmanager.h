/*
 *  Program Name    :  Admin
 *  File Name       :  productmanager.h
 *  Description     :  상품관리 위젯
 *                      -> 등록된 상품 목록 출력
 *                      -> 신규등록 요청 수행
 *                      -> 검색 요청 결과 전송
 *                      -> 정보수정 요청 수행
 *                      -> 주문클래스로 상품정보 전송
*/

#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H

#include <QWidget>

class ProductInput;
class QSqlQueryModel;

namespace Ui {
class ProductManager;
}

class ProductManager : public QWidget
{
    Q_OBJECT

public:
    explicit ProductManager(QWidget *parent = nullptr);
    ~ProductManager();

    void updateTable();

signals:
    void sendCurrentPK(int);
    void sendProductKey(QVector<int>);
    void sendResultPrice(QStringList);

private slots:
    void on_inputButton_clicked();              // Show the ProductInput Widget
    void update();
    void on_totalButton_clicked();
    void on_searchComboBox_currentIndexChanged(int index);
    void on_productTableView_clicked(const QModelIndex &index);
    void on_searchButton_clicked();
    void on_clearButton_clicked();
    void on_editButton_clicked();
    void recvPk(QString);           //가격 체크할 pk받기 from order

private:

    Ui::ProductManager *ui;

    ProductInput *productInput;             // Objects of Class ProductInput for function what input

    QSqlQueryModel *productQueryModel;

    QVector<int> stockOutVector;
};

#endif // PRODUCTMANAGER_H
