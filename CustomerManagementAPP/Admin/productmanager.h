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

class Product;
class ProductInput;
class ProductSearch;
class ProductEdit;
class QTreeWidgetItem;

namespace Ui {
class ProductManager;
}

class ProductManager : public QWidget
{
    Q_OBJECT

public:
    explicit ProductManager(QWidget *parent = nullptr);
    ~ProductManager();

    void loadData();            // Load registerd Product List

signals:
    void sendProductKey(QString, QString, bool);    // Send ProductKey to OrderManager and ChatManager
    void searchResult(QString, QString, QString, QString);
                                            // Send search result to ProductSearch and ProductEdit
    void changePrice(QString, QString);     // Send Price changed to OrderManager

private slots:
    int makeIndex();							// Make Index for Key of Map(ProductList)
    int makeProductKey(int, QString, int);		// Make CustomerKey by using index, Type, price

    void on_inputButton_clicked();              // Show the ProductInput Widget
    void on_searchButton_clicked();             // Show the ProductSearch Widget
    void on_editButton_clicked();               // Show the ProductEdit Widget
    void on_productTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
                                                // Slot for Remove Item (Product)

    // Receive the data about new Product's information from ProductInput
    void inputData(QString, QString, QString);  // Add new Product

    // Slots for Search
    void checkSearch(int, QString);             // Receive ProductKey from ProductSearch for search
    void checkSearch(int);                      // Check ProductKey and emit signal searchResult()

    // Slots for Edit
    void recvPK(QString);                       // Receive the ProductKey from ProductEdit for check
    void recvEditResult(QString, QString, QString, QString);
                                                // Receive the result of edit from ProductEdit
private:
    Ui::ProductManager *ui;

    QMap<int, Product*> productList;        // <Index, Product>

    ProductInput *productInput;             // Objects of Class ProductInput for function what input
    ProductSearch *productSearch;           // Objects of Class ProductSearch for function what input
    ProductEdit *productEdit;               // Objects of Class ProductEdit for function what input

    QList<Product*> searchList;             // list for sending SearchList for a time
};

#endif // PRODUCTMANAGER_H
