/*
 *  Program Name    :  Admin
 *  File Name       :  chatmanager.h
 *  Description     :  채팅관리 클래스
 *                      -> 채팅 및 파일 서버 구성
 *                      -> 로그 기록 출력
 *                      -> 공지사항 저장
 *                      -> 고객의 서버 접속상태 출력
*/

#ifndef CHATMANAGER_H
#define CHATMANAGER_H

#include <QWidget>
#include <QDataStream>

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

    void updateCustomerList();
    void updateNotice();
    void updateFileList();

private slots:
    //chat
    void sockConnect();                                                   // New Connection with Client
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
    QTcpServer *chatServer;                                     // Server for Chat
    QTcpServer *fileServer;                                     // Server for FileSending

    QTcpSocket *chatAdminSocket;                                // ChatAdmin Socket

    QList<QTcpSocket*> customerSocketList;                      // CustomerSocketList
    QHash<QString, QTcpSocket*> customerWaitSocketHash;         // <CustomerKey : CustomerSocket> -> Connected Socket List
    QHash<QString, QTcpSocket*> customerChatSocketHash;         // <CustomerKey : CustomerSocket> -> Chatting Socket List
                                                                // Admin can be connected to maximum two clients
    //File
    QFile* file;                                                // File Object for FileSending Protocol
    QProgressDialog* progressDialog;                            // Object for Showing Progress
    qint64 totalSize;                                           // Total size of File that clients are sending
    qint64 byteReceived = 0;                                    // size of File read currently
    QByteArray inBlock;                                         // Units divided to transfer files
    QString fileName;
    QString fileSender;
    QString checkFileName;


    //Thread
    LogSaveThread* logSaveThread;            // Thread Object for saving the log(chat) by using multi-threading

    QSqlQueryModel *customerModel;
    QSqlQueryModel *fileModel;
    QSqlQueryModel *noticeModel;

    QVector<int> waitVector;
    QVector<int> chattingVector;
};

#endif // CHATMANAGER_H
