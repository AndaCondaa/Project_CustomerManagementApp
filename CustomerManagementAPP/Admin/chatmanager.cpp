#include "chatmanager.h"
#include "ui_chatmanager.h"
#include "logsavethread.h"
#include "chatdelegate.h"

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

#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>


ChatManager::ChatManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatManager)
{
    ui->setupUi(this);
    chatAdminSocket = nullptr;

    // Open Server for chat and file transfer system
    // Chat Server
    chatServer = new QTcpServer(this);
    connect(chatServer, SIGNAL(newConnection()), SLOT(sockConnect()));
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
    connect(ui->logSaveButton, SIGNAL(clicked()),
            logSaveThread, SLOT(saveData()), Qt::QueuedConnection);

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

    updateCustomerList();
}

ChatManager::~ChatManager()
{
    delete ui;
    chatServer->close();
    fileServer->close();
    logSaveThread->terminate();
}

// New Connection with Client
void ChatManager::sockConnect()
{
    QTcpSocket *sockConnection = chatServer->nextPendingConnection();
    connect(sockConnection, SIGNAL(readyRead()), SLOT(receiveData()));
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
    if (chatAdminSocket == receiveSocket) {
        receiveFromAdmin(receiveSocket);
        return;
    } else if (customerSocketList.contains(receiveSocket)) {
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
            chatAdminSocket = new QTcpSocket;
            chatAdminSocket = receiveSocket;
            QString sendData;
            if(int count = customerWaitSocketHash.count()) {
                sendData.append(QString::number(count)+"||");
                foreach (auto ck, customerWaitSocketHash.keys()) {
                    sendData.append(ck+"|");
                }
            }
            sendProtocol(receiveSocket, Admin_In, sendData);
        } else {
            sendProtocol(receiveSocket, Admin_In_Fail, data);
        }
        break;
    }
    case Sign_In: {
        //Verification of CustomerKey & Name inputted by customer
        QList<QString> ckName = ((QString)data).split("|");  // In case, data = "customerKey|name"
        QModelIndexList indexes = customerModel->match(customerModel->index(0, 0), Qt::DisplayRole, ckName[0], 1, Qt::MatchFlags(Qt::MatchCaseSensitive));

        // Case of fail
        // Case of Already connected customer
        if (customerChatSocketHash.contains(ckName[0]) || customerWaitSocketHash.contains(ckName[0])) {
            sendProtocol(receiveSocket, Sign_In_Fail, data);
        } else if (indexes.empty()) {   //ck??? ?????? ??????
            sendProtocol(receiveSocket, Sign_In_Fail, "fail");
            return;
        } else {    // Case of wrong name
            foreach (auto idx, indexes) {
                if (ckName[1] != customerModel->data(idx.siblingAtColumn(1)).toString()) {
                    sendProtocol(receiveSocket, Sign_In_Fail, "fail");
                    return;
                }
            }
        }

        // Connection succeed
        customerSocketList.append(receiveSocket);                  // Insert socket into waiting_list
        customerWaitSocketHash.insert(ckName[0], receiveSocket);
        sendProtocol(receiveSocket, Sign_In, data);        // Return to customer about success
        if (chatAdminSocket != nullptr)
            sendProtocol(chatAdminSocket, Sign_In, data);
        updateCustomerList();
        break;
    }
    }
}

// Receive Data from AdminChat
void ChatManager::receiveFromAdmin(QTcpSocket* receiveSocket)
{

    Protocol_Type type;
    char data[1020];
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
        for(int i = 0; i < ui->logTreeWidget->columnCount(); i++) {
            ui->logTreeWidget->resizeColumnToContents(i);
        }

        logSaveThread->appendData(log);
        sendProtocol(customerChatSocketHash[ck], Message, msg);
        break;
    } //case Message
    case Invite: {  // data = customerKey
        customerChatSocketHash.insert(data, customerWaitSocketHash.value(data));
        customerWaitSocketHash.remove(data);

        QString inviteMsg =
                "<font color=orange><b>\n Hello! This is Osstem Implant! <b></font>";
        sendProtocol(customerChatSocketHash[data], Invite, inviteMsg);
        updateCustomerList();
        break;
    } //case Invite
    case Kick_Out: {
        customerWaitSocketHash.insert(data, customerChatSocketHash.value(data));
        customerChatSocketHash.remove(data);

        sendProtocol(customerWaitSocketHash[data], Kick_Out, data);
        updateCustomerList();
        break;
    } //case Kick_Out
    case Notice: {
        foreach (auto customerSocket, customerSocketList) {
            sendProtocol(customerSocket, Notice, data);
        }
        sendProtocol(chatAdminSocket, Notice, data);
        updateNotice();
        break;
    } // case Notice
    case Close: {
        for (auto i = customerChatSocketHash.begin(); i != customerChatSocketHash.end(); i++) {
            sendProtocol(i.value(), Close, "Admin Close"); // Notify Customer that Admin disconnected
        }
        receiveSocket->deleteLater();
        chatAdminSocket = nullptr;
        break;
    } //case Close
    } //switch
}

// Receive Data from Client
void ChatManager::receiveFromClient(QTcpSocket* receiveSocket)
{
    Protocol_Type type;
    char data[1020];
    memset(data, 0, 1020);

    QByteArray bytearray = receiveSocket->read(1024);
    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in.device()->seek(0);
    in >> type;
    in.readRawData(data, 1020);

    switch (type) {
    case In: {
        bool isIn = false;
        if (chatAdminSocket != nullptr) {
            if (customerChatSocketHash.count() < 2) {
                customerWaitSocketHash.remove(data);
                customerChatSocketHash.insert(data, receiveSocket);
                isIn = true;          // When There is AdminSocket for connection
            }
        }

        if (isIn) {   // Case of connection success
            type = In;
            QString clientMsg = "<font color=orange><b>\n Hello! This is Osstem Implant! <b></font>";
            sendProtocol(chatAdminSocket, type, data);    // To ChatAdmin
            sendProtocol(receiveSocket, type, clientMsg);
            updateCustomerList();
        } else {
            type = In_Fail;
            sendProtocol(receiveSocket, type, "Fail");
        }
        break;
    }
    case Message: {
        QString from = customerChatSocketHash.key(receiveSocket);
        QTreeWidgetItem* log = new QTreeWidgetItem;
        log->setText(0, receiveSocket->peerAddress().toString());
        log->setText(1, QString::number(receiveSocket->peerPort()));
        log->setText(2, from);
        log->setText(3, "ADMIN");
        log->setText(4, QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        log->setText(5, data);
        log->setToolTip(5, data);
        ui->logTreeWidget->addTopLevelItem(log);
        ui->logTreeWidget->resizeColumnToContents(4);
        sendProtocol(chatAdminSocket, Message, from + "|" + data);
        break;
    }
    case Out: { //data = CustomerKey
        bool isIn = customerChatSocketHash.remove(data);
        if (isIn) {
            customerWaitSocketHash[data] = receiveSocket;
        }
        sendProtocol(chatAdminSocket, Out, data);
        updateCustomerList();
        break;
    }
    case Close: {
        if (chatAdminSocket == nullptr)
            return;
        sendProtocol(chatAdminSocket, Close, data);
        customerSocketList.removeAll(receiveSocket);
        customerChatSocketHash.remove(data);
        customerWaitSocketHash.remove(data);
        receiveSocket->deleteLater();
        updateCustomerList();
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

    // Beginning File Transfer
    if (byteReceived == 0) {        // First Time(Block) , var byteReceived is always zero
        progressDialog->reset();
        progressDialog->show();

        checkFileName = fileName;

        QDataStream in(receivedSocket);
        in.device()->seek(0);
        in >> totalSize >> byteReceived >> fileName >> fileSender;

        if(checkFileName == fileName) return;

        progressDialog->setMaximum(totalSize);

        QFileInfo info(fileName);
        QString currentFileName = info.fileName();
        file = new QFile("../Admin/data/file/" + currentFileName);
        file->open(QFile::WriteOnly);
    } else {
        if(checkFileName == fileName) return;

        inBlock = receivedSocket->readAll();

        byteReceived += inBlock.size();
        file->write(inBlock);
        file->flush();
    }
    progressDialog->setValue(byteReceived);
    if (byteReceived == totalSize) {        // file sending is done
        qDebug() << QString("%1 receive completed").arg(fileName);

        // Insert file information into file_table
        QList<QString> list = fileName.split("/");      // To save only File name without path
        QSqlDatabase chatDB = QSqlDatabase::database("ChatManager");
        QSqlQuery inputFile(chatDB);
        inputFile.prepare("INSERT INTO sys.FILE_TABLE VALUES(:sender, :filename)");
        inputFile.bindValue(":sender", fileSender);
        inputFile.bindValue(":filename", list[list.count()-1]);
        bool isExec = inputFile.exec();
        if (isExec) updateFileList();

        inBlock.clear();
        byteReceived = 0;
        totalSize = 0;
        progressDialog->reset();
        progressDialog->hide();
        file->close();
        delete file;
    }
}

// Update NoticeTableView
void ChatManager::updateNotice()
{
    QSqlDatabase chatDB = QSqlDatabase::database("ChatManager");
    noticeModel->setQuery("SELECT NOTICE_CONTENTS FROM sys.NOTICE_TABLE ORDER BY NOTICE_DATE", chatDB);
    noticeModel->setHeaderData(0, Qt::Horizontal, tr("CONTENTS"));

    ui->noticeTableView->setModel(noticeModel);
    ui->noticeTableView->horizontalHeader()->setStretchLastSection(true);
    ui->noticeTableView->horizontalHeader()->setStyleSheet(
                "QHeaderView { font-size: 10pt; color: blue; }");
}

// Update CustomerTableView
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

    waitVector.clear();
    chattingVector.clear();
    foreach(auto waitCk, customerWaitSocketHash.keys()) {
        QModelIndexList indexes = customerModel->match(customerModel->index(0, 0), Qt::EditRole, waitCk, -1, Qt::MatchFlags(Qt::MatchExactly));
        foreach (auto idx, indexes) {
            waitVector.append(idx.row());
        }
    }
    foreach(auto chatCk, customerChatSocketHash.keys()) {
        QModelIndexList indexes = customerModel->match(customerModel->index(0, 0), Qt::EditRole, chatCk, -1, Qt::MatchFlags(Qt::MatchExactly));
        foreach (auto idx, indexes) {
            chattingVector.append(idx.row());
        }
    }

    ChatDelegate *delegate = new ChatDelegate(ui->customerTableView);
    delegate->setWaitVector(waitVector);
    delegate->setChattingVector(chattingVector);
    ui->customerTableView->setItemDelegateForColumn(0, delegate);
    ui->customerTableView->setItemDelegateForColumn(1, delegate);
}

// Update FileTableView
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

