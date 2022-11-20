/*
 *  Program Name    :  Admin
 *  File Name       :  logsavethread.h
 *  Description     :  Thread for saving Log
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
