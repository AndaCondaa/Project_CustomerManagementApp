/*
 *  Program Name    :  Admin
 *  File Name       :  logsavethread.h
 *  Description     :  채팅 로그 기록을 주기적으로 저장하는 쓰레드
*/

#ifndef LOGSAVETHREAD_H
#define LOGSAVETHREAD_H

#include <QThread>
#include <QList>

class QTreeWidgetItem;

class LogSaveThread : public QThread
{
    Q_OBJECT
public:
    explicit LogSaveThread(QObject *parent = nullptr);

public slots:
    void run();                                 // Thread start and save
    void appendData(QTreeWidgetItem*);          // Append item at itemList
    void saveData();                            // Save the log


private:
    QList<QTreeWidgetItem*> itemList;           // Logs
    QString filename;                           // File name for saving



};

#endif // LOGSAVETHREAD_H
