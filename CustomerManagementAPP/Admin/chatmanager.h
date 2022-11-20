/*
 *  Program Name    :  Admin
 *  File Name       :  chatmanager.h
 *  Description     :  Management for Chat
*/

#ifndef CHATMANAGER_H
#define CHATMANAGER_H

#include <QWidget>

class QTcpServer;
class QTcpSocket;
class LogSaveThread;
class QFile;
class QProgressDialog;
class QSqlQueryModel;

typedef enum {      //Protocol Types for Chat
    Sign_In,
    Sign_In_Fail,
    Admin_In,
    Admin_In_Fail,
    In,
    In_Fail,
    Out,
    Message,
    Invite,
    Kick_Out,
    Close,
    Notice
} Protocol_Type;

namespace Ui {
class ChatManager;
}

class ChatManager : public QWidget
{
    Q_OBJECT

public:
    explicit ChatManager(QWidget *parent = nullptr);
    ~ChatManager();

    void updateCustomerList();                                  // Update CustomerTableView
    void updateNotice();                                        // Update NoticeTableView
    void updateFileList();                                      // Update FileTableView

private slots:
    //chat
    void sockConnect();                                                     // New Connection with Client
    void sendProtocol(QTcpSocket* ,Protocol_Type, QString, int = 1020);     // Send Socket To Client
    void receiveData();              // Receive Data for Connection From AdminChat(Program) & Client(Program)
    void receiveFromAdmin(QTcpSocket*);                                     // Receive Data from AdminChat
    void receiveFromClient(QTcpSocket*);                                    // Receive Data from Client

    //file
    void acceptConnection();         // Make Socket for FileSending and Connect signal/slot
    void readClient();               // Read Socket sent from Client and AdaminChat for saving file

private:
    Ui::ChatManager *ui;

    //Server
    QTcpServer *chatServer;                                 // Server for Chat
    QTcpServer *fileServer;                                 // Server for FileSending

    QTcpSocket *chatAdminSocket;                            // ChatAdmin Socket
    QList<QTcpSocket*> customerSocketList;                  // CustomerSocketList
    QHash<QString, QTcpSocket*> customerWaitSocketHash;     // <CustomerKey : CustomerSocket> -> Connected Socket List
    QHash<QString, QTcpSocket*> customerChatSocketHash;     // <CustomerKey : CustomerSocket> -> Chatting Socket List

    //File
    QFile* file;                                // File Object for FileSending Protocol
    QProgressDialog* progressDialog;            // Object for Showing Progress
    qint64 totalSize;                           // Total size of File that clients are sending
    qint64 byteReceived = 0;                    // size of File read currently
    QByteArray inBlock;                         // Units divided to transfer files
    QString fileName;                           // Receiving FileName
    QString fileSender;                         // Receiving File's Sender
    QString checkFileName;                      // Previous File Name for checking new file

    //Thread
    LogSaveThread* logSaveThread;            // Thread Object for saving the log(chat) by using multi-threading

    //Model
    QSqlQueryModel *customerModel;           // Model for CustomerTableView
    QSqlQueryModel *fileModel;               // Model for FileListView
    QSqlQueryModel *noticeModel;             // Modle for NoticeListView

    //Vectors for Delegate
    QVector<int> waitVector;                 // Waitting Customer's row number List in Model
    QVector<int> chattingVector;             // Chatting Customer's row number List in Model
};

#endif // CHATMANAGER_H
