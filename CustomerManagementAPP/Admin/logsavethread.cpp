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
        sleep(60);      // 60senconds
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
