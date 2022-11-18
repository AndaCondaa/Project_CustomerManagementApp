/*
 *  Program Name    :  Client
 *  File Name       :  client.h
 *  Description     :  고객용 채팅 프로그램
 *                      -> 채팅 기능
 *                      -> 파일 전송 기능
*/


#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>

class QTcpSocket;
class QFile;
class QProgressDialog;
class QSqlQueryModel;

typedef enum {       //Protocol Types for Chat
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

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QWidget
{
    Q_OBJECT

public:
    Client(QWidget *parent = nullptr, QString ck = "", QString clinic = "");
    ~Client();

private slots:
    // Chat
    void sendProtocol(Protocol_Type, QString, int = 1020);      // Send data to server
    void receiveData();			// Receive the data from server
    void sendData();			// Setting the data when send it to server

    // File
    void goOnSend(qint64);      // Seperate file for sending
    void sendFile();            // File sending

    //Notice
    void updateNotice();          //
    void disconnect();
    void on_outButton_clicked();

    void on_chatButton_clicked();

private:
    void closeEvent(QCloseEvent*) override;     // If Client Programs are closed, send data to server
    void saveLog();
    void loadLog();

    Ui::Client *ui;
    QTcpSocket *clientSocket;		// Socket for Client
    QTcpSocket *fileSocket;         // Socket for File sending
    QProgressDialog* progressDialog;   // Check File sending progress
    QFile* file;                    // File sending to server
    qint64 loadSize;                // File Size
    qint64 byteToWrite;             // File Size per a block
    qint64 totalSize;               // Total File Size
    QByteArray outBlock;            // Block for sending
    bool isSent = false;            // Check connection to server

    QSqlQueryModel *noticeModel;
};
#endif // CLIENT_H
