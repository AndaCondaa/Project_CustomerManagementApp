#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

//For SQL
#include <QApplication>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>


static bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("Oracle11gx64");
    db.setUserName("project");
    db.setPassword("project");
    if (!db.open()) {
        qDebug() << db.lastError().text();
    } else {
        qDebug("success");
    }

    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    createConnection();

    QTranslator translator;
    translator.load("://admintr_ko.qm");
    QApplication::installTranslator(&translator);
    MainWindow w;
    w.show();
    return a.exec();
}
