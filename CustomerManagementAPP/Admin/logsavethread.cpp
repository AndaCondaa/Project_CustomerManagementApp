/*
 *  Program Name    :  Admin
 *  File Name       :  logsavethread.cpp
 *  Description     :  채팅 로그 기록을 주기적으로 저장하는 쓰레드
*/


#include "logsavethread.h"

#include <QTreeWidgetItem>
#include <QFile>
#include <QDateTime>

LogSaveThread::LogSaveThread(QObject *parent)
    : QThread{parent}
{
    QString format = "yyyyMMdd_hhmmss";
    filename = QString("../Admin/data/log/log_%1.txt").arg(QDateTime::currentDateTime().toString(format));
}

// Thread start and save
void LogSaveThread::run()
{
    Q_FOREVER {
        saveData();
        sleep(60);      // 1분마다 저장
    }
}

// Append item at itemList
void LogSaveThread::appendData(QTreeWidgetItem* item)
{
    itemList.append(item);
}

// Save the log
void LogSaveThread::saveData()
{
    if(itemList.count() > 0) {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream out(&file);
        foreach(auto item, itemList) {
            out << item->text(0) << ", ";
            out << item->text(1) << ", ";
            out << item->text(2) << ", ";
            out << item->text(3) << ", ";
            out << item->text(4) << ", ";
            out << item->text(5) << "\n";
        }
        file.close();
    }
    qDebug() << "LOG SAVED!";
}
