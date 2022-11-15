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

#include <QApplication>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include <QColor>


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

    qDebug() << tr("Server Open");


    QSqlDatabase chatDB = QSqlDatabase::addDatabase("QODBC", "ChatManager");
    chatDB.setDatabaseName("Oracle11gx64");
    chatDB.setUserName("chat_manager");
    chatDB.setPassword("chat");
    if (!chatDB.open()) {
        qDebug() << chatDB.lastError().text();
    } else {
        qDebug("Chat DB connect success");
    }

    customerModel = new QSqlQueryModel(ui->customerTableView);
    fileModel = new QSqlQueryModel(ui->fileTableView);
    noticeModel = new QSqlQueryModel(ui->noticeTableView);

}

ChatManager::~ChatManager()
{
    delete ui;
    chatServer->close();
    fileServer->close();
    logSaveThread->terminate();
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

        //customerModel에서 로그인요청한 ck|name 검증
        QList<QString> ckName = ((QString)data).split("|"); // data = "customerKey|name"
        QModelIndexList indexes = customerModel->match(customerModel->index(0, 0), Qt::EditRole, ckName[0], 1, Qt::MatchFlags(Qt::MatchExactly));
        // 로그인 실패
        foreach (auto idx, indexes) {
            // ck는 맞지만, 이름이 틀린 경우
            if (ckName[1] != customerModel->data(idx.siblingAtColumn(1)).toString()) {
                sendProtocol(receiveSocket, Sign_In_Fail, "fail");
                return;
            }
            // 이름까지 맞는 경우 리스트 색상변경
            QColor co(100,0,123);
            customerModel->setData(idx, co, Qt::BackgroundRole);
        }
        // 로그인 성공 시
        customerSocketHash[ckName[0]] = receiveSocket;             // CustomerKey : TcpSocket
        customerSocketList.append(receiveSocket);                  // Insert socket into waiting_list

        sendProtocol(receiveSocket, Sign_In, "success");        //로그인한 해당 고객에게 성공여부 전달
        foreach (auto adminSock, adminSocketList)               //접속한 채팅관리자들에게 로그인한 고객정보 전송
            sendProtocol(adminSock, Sign_In, data);
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
        //초대한 고객 색상 변경
        QModelIndexList indexes = customerModel->match(customerModel->index(0, 0), Qt::EditRole, data, 1, Qt::MatchFlags(Qt::MatchExactly));
        foreach (auto idx, indexes) {
            ui->customerTableView->indexWidget(idx)->setStyleSheet("{background-color: red}");
        }

        QString inviteMsg =
                "<font color=orange><b> 안녕하세요. 오스템임플란트입니다. 상담원과 연결되었습니다! <b></font>";
        customerMatchingMap.insert(receiveSocket,customerSocketHash[data]);
        sendProtocol(customerSocketHash[data], Invite, inviteMsg);
        break;
    }
    case Kick_Out: {
        //강퇴한 고객 색상 변경
        QModelIndexList indexes = customerModel->match(customerModel->index(0, 0), Qt::EditRole, data, 1, Qt::MatchFlags(Qt::MatchExactly));
        foreach (auto idx, indexes) {
            ui->customerTableView->indexWidget(idx)->setStyleSheet("{background-color: yellow}");
        }

        sendProtocol(customerSocketHash[data], Kick_Out, data);
        customerMatchingMap.remove(receiveSocket, customerSocketHash[data]);
        break;
    }
    case Notice:
        foreach (auto customerSocket, customerSocketList) {
            sendProtocol(customerSocket, Notice, data);
        }
        foreach (auto adminSocket, adminSocketList) {
            if (adminSocket != receiveSocket)
                sendProtocol(adminSocket, Notice, data);
        }
        updateNotice();
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
        if (op) {   // 연결가능한 AdminChat을 찾아 연결되었을 때,
            type = In;
            clientMsg = "<font color=orange><b>\n 안녕하세요. 오스템임플란트입니다. 상담원과 연결되었습니다! <b></font>";
            QModelIndexList indexes = customerModel->match(customerModel->index(0, 0), Qt::EditRole, data, 1, Qt::MatchFlags(Qt::MatchExactly));
//            foreach (auto idx, indexes) {
//                ui->customerTableView->indexWidget(idx)->setStyleSheet("{background-color: red}");
//            }
            sendProtocol(customerMatchingMap.key(receiveSocket), type, data);    //담당 Admin에게 전송
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
        log->setText(4, QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        log->setText(5, ((QString)data).split("|")[1]);
        log->setToolTip(5, QString(data));
        ui->logTreeWidget->addTopLevelItem(log);
        ui->logTreeWidget->resizeColumnToContents(4);
        sendProtocol(customerMatchingMap.key(receiveSocket), Message, data);
        break;
    }
    case Out: {
        QModelIndexList indexes = customerModel->match(customerModel->index(0, 0), Qt::EditRole, data, 1, Qt::MatchFlags(Qt::MatchExactly));
//        foreach (auto idx, indexes) {
//            ui->customerTableView->indexWidget(idx)->setStyleSheet("{background-color: yellow}");
//        }

        sendProtocol(customerMatchingMap.key(receiveSocket), Out, data);
        customerMatchingMap.remove(customerMatchingMap.key(receiveSocket), receiveSocket);
        break;
    }
    case Close: {
        QModelIndexList indexes = customerModel->match(customerModel->index(0, 0), Qt::EditRole, data, 1, Qt::MatchFlags(Qt::MatchExactly));
//        foreach (auto idx, indexes) {
//            ui->customerTableView->indexWidget(idx)->setStyleSheet("{background-color: yellow}");
//        }

        foreach (auto admin, adminSocketList) {
            sendProtocol(admin, Close, data);
        }
        customerMatchingMap.remove(customerMatchingMap.key(receiveSocket), receiveSocket);
        customerSocketHash.remove(data);

        customerSocketList.removeAll(receiveSocket);
        customerSocketList.squeeze();
        receiveSocket->deleteLater();
        break;
    }
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

        //파일 디비에 저장하는 코드 작성
//        ui->fileTreeWidget->addTopLevelItem(fileLog);
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

void ChatManager::updateNotice()
{
    QSqlDatabase chatDB = QSqlDatabase::database("ChatManager");
    noticeModel->setQuery("SELECT * FROM sys.NOTICE_TABLE ORDER BY NOTICE_DATE", chatDB);
    noticeModel->setHeaderData(0, Qt::Horizontal, tr("DATE"));
    noticeModel->setHeaderData(1, Qt::Horizontal, tr("NOTICE"));

    ui->noticeTableView->setModel(noticeModel);
    ui->noticeTableView->horizontalHeader()->setStretchLastSection(true);
    ui->noticeTableView->horizontalHeader()->setStyleSheet(
                "QHeaderView { font-size: 10pt; color: blue; }");
}

void ChatManager::updateCustomerList()
{
    QSqlDatabase chatDB = QSqlDatabase::database("ChatManager");
    customerModel->setQuery("SELECT CUSTOMER_KEY, CLINIC_NAME FROM sys.CUSTOMER_TABLE ORDER BY 1", chatDB);
    customerModel->setHeaderData(0, Qt::Horizontal, tr("CUSTOMER_KEY"));
    customerModel->setHeaderData(1, Qt::Horizontal, tr("CLINIC"));

    ui->customerTableView->setModel(customerModel);
    ui->customerTableView->horizontalHeader()->setStretchLastSection(true);
    ui->customerTableView->horizontalHeader()->setStyleSheet(
                "QHeaderView { font-size: 10pt; color: blue; }");
}

void ChatManager::updateFileList()
{
    QSqlDatabase chatDB = QSqlDatabase::database("ChatManager");
    fileModel->setQuery("SELECT * FROM sys.FILE_TABLE", chatDB);
    fileModel->setHeaderData(0, Qt::Horizontal, tr("FROM"));
    fileModel->setHeaderData(1, Qt::Horizontal, tr("FILE NAME"));
    ui->fileTableView->setModel(fileModel);
    ui->fileTableView->horizontalHeader()->setStretchLastSection(true);
    ui->fileTableView->horizontalHeader()->setStyleSheet(
                "QHeaderView { font-size: 10pt; color: blue; }");
}
