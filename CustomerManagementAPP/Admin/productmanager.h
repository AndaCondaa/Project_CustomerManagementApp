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
#include <QMap>

class ProductInput;
class ProductSearch;
class ProductEdit;


namespace Ui {
class ProductManager;
}

class ProductManager : public QWidget
{
    Q_OBJECT

public:
    explicit ProductManager(QWidget *parent = nullptr);
    ~ProductManager();

private slots:
    void on_inputButton_clicked();              // Show the ProductInput Widget
    void on_searchButton_clicked();             // Show the ProductSearch Widget
    void on_editButton_clicked();               // Show the ProductEdit Widget

private:
    Ui::ProductManager *ui;

    ProductInput *productInput;             // Objects of Class ProductInput for function what input
    ProductSearch *productSearch;           // Objects of Class ProductSearch for function what input
    ProductEdit *productEdit;               // Objects of Class ProductEdit for function what input
};

#endif // PRODUCTMANAGER_H
