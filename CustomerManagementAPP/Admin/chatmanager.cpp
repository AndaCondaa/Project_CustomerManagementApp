/*
 *  Program Name    :  Admin
 *  File Name       :  chatmanager.cpp
 *  Description     :  채팅관리 클래스
 *                      -> 서버 구성
 *                      -> 로그 기록 출력
 *                      -> 공지사항 저장
 *                      -> 고객의 서버 접속상태 출력
*/

#include "chatmanager.h"
#include "ui_chatmanager.h"
#include "logsavethread.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QApplication>
#include <QThread>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>
#include <QScrollBar>
#include <QDateTime>
#include <QDebug>
#include <QLineEdit>

ChatManager::ChatManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatManager)
{
    ui->setupUi(this);

    // Open Server for chat and file transfer system
    // Chat Server
    chatServer = new QTcpServer(this);
    connect(chatServer, SIGNAL(newConnection()), SLOT(clientConnect()));
    if (!chatServer->listen(QHostAddress::Any, 8000)) {    //Using 8000 as a fixed port for chat
        QMessageBox::critical(this, tr("Chatting Server"), \
                              tr("Unable to start the server: %1.") \
                              .arg(chatServer->errorString( )));
        close();
        return;
    }

    // File Server
    fileServer = new QTcpServer(this);
    connect(fileServer, SIGNAL(newConnection()), SLOT(acceptConnection()));
    if (!fileServer->listen(QHostAddress::Any, 8000+1)) {   //Using 8001 as a fixed port for chat
        QMessageBox::critical(this, tr("Chatting Server"), \
                              tr("Unable to start the server: %1.") \
                              .arg(fileServer->errorString( )));
        close( );
        return;
    }

    // Object for Showing Progress
    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();

    // Thread Object for saving the log(chat) by using multi-threading
    logSaveThread = new LogSaveThread(this);
    logSaveThread->start();
    connect(ui->logSaveButton, SIGNAL(clicked()), logSaveThread, SLOT(saveData()));

    // Load the Previous Notice from file(.txt)
    noticeLoad();

    qDebug() << "Server Open";
}

ChatManager::~ChatManager()
{
    noticeSave();       // Save the Notice posted until now before crashing the ui

    delete ui;
    chatServer->close();
    fileServer->close();
    logSaveThread->terminate();
}

// Receive CustomerKey and Name from CustomerManger
void ChatManager::recvCustomerKey(QString ck, QString clinic, bool check)
{
    if (check) { // True means that Customer added
        QTreeWidgetItem *customer = new QTreeWidgetItem;
        customer->setText(0, ck);
        customer->setText(1, clinic);
        customer->setText(2, tr("Disconnected"));
        ui->customerTreeWidget->addTopLevelItem(customer);
        customerNameHash[ck] = clinic;
    } else { // False means that Customer deleted
        foreach(auto item, ui->customerTreeWidget->findItems(ck, Qt::MatchExactly, 0)) {
            ui->customerTreeWidget->takeTopLevelItem(
                        ui->customerTreeWidget->indexOfTopLevelItem(item));
            customerNameHash.remove(ck);
        }
    }
}

// Save the Notice to file(.txt)
void ChatManager::noticeSave()
{
    QFile file("../Admin/data/manage/notice.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QByteArray notice;
    notice.append(ui->noticeBoard->toPlainText().toStdString());
    file.write(notice);
    file.close( );
}

// Load the Previous Notice from file(.txt)
void ChatManager::noticeLoad()
{
    QFile file("../Admin/data/manage/notice.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QByteArray notice = file.readAll();
    ui->noticeBoard->setPlainText(notice);

    file.close( );
}


// New Connection with Client
void ChatManager::clientConnect()
{
    QTcpSocket *customerConnection = chatServer->nextPendingConnection();
    connect(customerConnection, SIGNAL(readyRead()), SLOT(receiveData()));
    qDebug() << "New Connection with Client";
}

// Send Socket To Client
void ChatManager::sendProtocol(QTcpSocket* sock, Protocol_Type type, QString data, int size)
{
    QByteArray dataArray;
    QDataStream out(&dataArray, QIODevice::WriteOnly);
    out.device()->seek(0);
    out << type;
    out.writeRawData(data.toStdString().data(), size);
    sock->write(dataArray);
    sock->flush();
    while(sock->waitForBytesWritten());
}

// Receive Data for Connection From AdminChat(Program) & Client(Program)
void ChatManager::receiveData()
{
    QTcpSocket *receiveSocket = dynamic_cast<QTcpSocket *>(sender());
    if (receiveSocket->bytesAvailable() > 1024) return;
    if (adminSocketList.contains(receiveSocket)) {
        receiveFromAdmin(receiveSocket);
        return;
    }
    else if (customerSocketList.contains(receiveSocket)) {
        receiveFromClient(receiveSocket);
        return;
    }

    QByteArray bytearray = receiveSocket->read(1024);
    Protocol_Type type;
    char data[1020];
    memset(data, 0, 1020);

    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in.device()->seek(0);
    in >> type;
    in.readRawData(data, 1020);

    switch(type) {
    case Admin_In: {
        QList<QString> adminCheck = ((QString)data).split("|");

        if ((adminCheck[0] == "admin") && (adminCheck[1] == "admin")) {
            adminSocketList.append(receiveSocket);
            sendProtocol(receiveSocket, Admin_In, data);
        } else
            sendProtocol(receiveSocket, Admin_In_Fail, data);
        break;
    }
    case Sign_In: {
        //Verification of CustomerKey & Name inputted by customer
        QList<QString> ckName = ((QString)data).split("|");

        type = Sign_In_Fail;
        foreach (auto item, ui->customerTreeWidget->findItems(ckName[0], Qt::MatchExactly, 0)) {
            if (item->text(1) == ckName[1] && item->text(2) == "Disconnected") {
                type = Sign_In;
                item->setText(2, "Connected");
                customerSocketHash[ckName[0]] = receiveSocket;             // CustomerKey : TcpSocket
                customerSocketList.append(receiveSocket);                    // Insert socket into waiting_list
                foreach (auto admin, adminSocketList) {
                    sendProtocol(admin, Sign_In, (QString)data + "|Connected");
                }
            }
        }
        sendProtocol(receiveSocket, type, data);
        break;
    }
    }
}

// Receive Data from AdminChat
void ChatManager::receiveFromAdmin(QTcpSocket* receiveSocket)
{
    Protocol_Type type;       // 채팅의 목적
    char data[1020];         // 전송되는 메시지/데이터
    memset(data, 0, 1020);

    QByteArray bytearray = receiveSocket->read(1024);
    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in.device()->seek(0);
    in >> type;
    in.readRawData(data, 1020);

    switch (type) {
    case Message: {
        QString ck = ((QString)data).split("||")[0];
        QString msg = ((QString)data).split("||")[1];

        QTreeWidgetItem* log = new QTreeWidgetItem;
        log->setText(0, receiveSocket->peerAddress().toString());
        log->setText(1, QString::number(receiveSocket->peerPort()));
        log->setText(2, "ADMIN");
        log->setText(3, ck);
        log->setText(4, QDateTime::currentDateTime().toString());
        log->setText(5, msg.split("</font>")[1]);
        log->setToolTip(5, QString(data));
        ui->logTreeWidget->addTopLevelItem(log);
        for(int i = 0; i < ui->logTreeWidget->columnCount(); i++)
            ui->logTreeWidget->resizeColumnToContents(i);

        logSaveThread->appendData(log);
        sendProtocol(customerSocketHash[ck], Message, msg);
        break;
    }
    case Invite: {
        foreach (auto item, ui->customerTreeWidget->findItems(data, Qt::MatchExactly, 0)) {
            item->setText(2, "Chatting...");
        }
        QString inviteMsg =
                "<font color=orange><b> 안녕하세요. 오스템임플란트입니다. 상담원과 연결되었습니다! <b></font>";
        customerMatchingMap.insert(receiveSocket,customerSocketHash[data]);
        sendProtocol(customerSocketHash[data], Invite, inviteMsg);
        break;
    }
    case Kick_Out:
        foreach (auto item, ui->customerTreeWidget->findItems(data, Qt::MatchExactly, 0)) {
            item->setText(2, "Connected");
        }
        sendProtocol(customerSocketHash[data], Kick_Out, data);
        customerMatchingMap.remove(receiveSocket, customerSocketHash[data]);
        break;
    case Notice:
        ui->noticeBoard->append(data);
        foreach (auto customerSocket, customerSocketList) {
            sendProtocol(customerSocket, Notice, data);
        }
        foreach (auto adminSocket, adminSocketList) {
            if (adminSocket != receiveSocket)
                sendProtocol(adminSocket, Notice, data);
        }
    }
}

// Receive Data from Client
void ChatManager::receiveFromClient(QTcpSocket* receiveSocket)
{
    Protocol_Type type;       // 채팅의 목적
    char data[1020];        // 전송되는 메시지/데이터
    memset(data, 0, 1020);

    QByteArray bytearray = receiveSocket->read(1024);
    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in.device()->seek(0);
    in >> type;
    in.readRawData(data, 1020);

    switch (type) {
    case In: {
        QString adminMsg = (QString)data + "_" + customerNameHash[data];      // 채팅연결된 경우, Client와 연결된 Admin에게 CustomerKey 전송
        QString clientMsg;            // 채팅연결 시도한 Client에게 보내는 메세지
        bool op = false;

        // Admin Socket 중에서 고객과 연결할 수 있는 admin 검색해서 연결, 하나의 admin 소켓 당 2개의 고객 socket 연결 제한
        foreach (auto admin, adminSocketList) {
            if (customerMatchingMap.count(admin) < 2) {
                customerMatchingMap.insert(admin, receiveSocket);
                op = true;          // 연결가능한 AdminSocket 검색 성공
                break;
            }
        }
        if (op) {
            type = In;
            clientMsg = "<font color=orange><b> 안녕하세요. 오스템임플란트입니다. 상담원과 연결되었습니다! <b></font>";
            foreach (auto item, ui->customerTreeWidget->findItems(data, Qt::MatchExactly, 0)) {
                item->setText(2, "Chatting...");
            }
            sendProtocol(customerMatchingMap.key(receiveSocket), type, adminMsg);    //담당 Admin에게 전송
        } else {
            type = In_Fail;
            clientMsg = "<font color=orange><b> 죄송합니다. 지금은 모든 상담원이 상담중입니다. <b></font>";
        }
        sendProtocol(receiveSocket, type, clientMsg);                           //해당 Client에게 전송
        break;
    }
    case Message: {
        QTreeWidgetItem* log = new QTreeWidgetItem;
        log->setText(0, receiveSocket->peerAddress().toString());
        log->setText(1, QString::number(receiveSocket->peerPort()));
        log->setText(2, customerSocketHash.key(receiveSocket));
        log->setText(3, "ADMIN");
        log->setText(4, QDateTime::currentDateTime().toString());
        log->setText(5, ((QString)data).split("|")[1]);
        log->setToolTip(5, QString(data));
        ui->logTreeWidget->addTopLevelItem(log);
        sendProtocol(customerMatchingMap.key(receiveSocket), Message, data);
        break;
    }
    case Out:
        foreach (auto item, ui->customerTreeWidget->findItems(data, Qt::MatchExactly, 0)) {
            item->setText(2, "Connected");
        }
        sendProtocol(customerMatchingMap.key(receiveSocket), Out, data);
        customerMatchingMap.remove(customerMatchingMap.key(receiveSocket), receiveSocket);
        break;
    case Close:
        foreach (auto item, ui->customerTreeWidget->findItems(data, Qt::MatchExactly, 0)) {
            item->setText(2, "Disconnected");
        }
        foreach (auto admin, adminSocketList) {
            sendProtocol(admin, Close, data);
        }

        customerMatchingMap.remove(customerMatchingMap.key(receiveSocket), receiveSocket);
        customerSocketHash.remove(data);

        customerSocketList.removeAll(receiveSocket);
        customerSocketList.squeeze();
        receiveSocket->deleteLater();
    }
}

// Make Socket for FileSending and Connect signal/slot
void ChatManager::acceptConnection()
{
    QTcpSocket* receivedSocket = fileServer->nextPendingConnection();
    connect(receivedSocket, SIGNAL(readyRead()), this, SLOT(readClient()));
}

// Read Socket sent from Client and AdaminChat for saving file
void ChatManager::readClient()
{
    QTcpSocket* receivedSocket = dynamic_cast<QTcpSocket *>(sender());
    QString filename, fileSender;

    // Beginning File Transfer
    if (byteReceived == 0) {        // First Time(Block) , var byteReceived is always zero
        progressDialog->reset();
        progressDialog->show();

        QDataStream in(receivedSocket);
        in >> totalSize >> byteReceived >> filename >> fileSender;

        progressDialog->setMaximum(totalSize);


        QList<QString> list = filename.split("/");      // To save only File name without path

        QTreeWidgetItem* fileLog = new QTreeWidgetItem;
        fileLog->setText(0, fileSender);
        fileLog->setText(1, list[list.count()-1]);
        fileLog->setToolTip(1, filename);

        // Resizing and Fixing ths siez of QTreeWidget
        for(int i = 0; i < ui->fileTreeWidget->columnCount(); i++)
            ui->fileTreeWidget->resizeColumnToContents(i);

        ui->fileTreeWidget->addTopLevelItem(fileLog);
        logSaveThread->appendData(fileLog);

        QFileInfo info(filename);
        QString currentFileName = info.fileName();
        file = new QFile("../Admin/data/file/" + currentFileName);
        file->open(QFile::WriteOnly);
    } else {                    // 파일 데이터를 읽어서 저장
        inBlock = receivedSocket->readAll();

        byteReceived += inBlock.size();
        file->write(inBlock);
        file->flush();
    }
    progressDialog->setValue(byteReceived);
    if (byteReceived == totalSize) {        /* 파일의 다 읽으면 QFile 객체를 닫고 삭제 */
        qDebug() << QString("%1 receive completed").arg(filename);

        inBlock.clear();
        byteReceived = 0;
        totalSize = 0;
        progressDialog->reset();
        progressDialog->hide();
        file->close();
        delete file;
    }
}
