/*
 *  Program Name    :  AdminChat
 *  File Name       :  adminchat.h
 *  Description     :  ChatProgram for Admin
*/

#ifndef ADMINCHAT_H
#define ADMINCHAT_H

#include <QWidget>

class QTcpSocket;
class QFile;
class QProgressDialog;
class QSqlQueryModel;

typedef enum {          //Protocol Types for Chat
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
namespace Ui { class AdminChat; }
QT_END_NAMESPACE

class AdminChat : public QWidget
{
    Q_OBJECT

public:
    AdminChat(QWidget *parent = nullptr);
    ~AdminChat();

signals:
    void message(QString);      // Receive the message from server and Open the Chat(new Tab) to the QTabWidget
    void tabClose();            // Signal for TabClose when Customer disconnected from server

private slots:
    // Chat
    void sendProtocol(Protocol_Type, QString, int = 1020);      // Send data to server
    void receiveData();                 // Receive the data from server
    void chatOpen(QString);             // Append new Tab of chat when new admin sign-in
    void on_inviteButton_clicked();     // Invite Customer
    void on_kickButton_clicked();       // Kick Out Customer

    // File
    void goOnSend(qint64);              // Seperate file for sending
    void sendFile();                    // File Sending
    void disconnect();                  // Call this function when disconnected from server

    //Notice
    void on_noticeButton_clicked();     // Append new notice

private:
    void closeEvent(QCloseEvent*) override;     // If Client Programs are closed, send data to server
    void updateCustomerList();          // Updating CustomerTableView
    void updateNotice();                // Updating NoticeTableView
    void insertNotice();                // Inserting new Notice
    void saveLog();                     // Save the log when Chatting is closed
    void loadLog();                     // Load the log when Chatting is opend

    Ui::AdminChat *ui;

    QTcpSocket *adminSocket;		// Socket for AdminChat
    QTcpSocket *fileSocket;         // Socket for File sending
    QProgressDialog* progressDialog;    // Check File sending progress
    QFile* file;                    // File sending to server
    qint64 loadSize;                // File Size
    qint64 byteToWrite;             // File Size per a block
    qint64 totalSize;               // Total File Size
    QByteArray outBlock;            // Block for sending
    bool isSent = false;            // Check connection to server

    QSqlQueryModel *customerModel;      // Model for CustomerTableView
    QSqlQueryModel *noticeModel;        // Model for NoticeTableView

    QVector<QString> waitVector;        // Waitting Customer's row number List in Model
    QVector<QString> chattingVector;    // Chatting Customer's row number List in Model
};
#endif // ADMINCHAT_H
