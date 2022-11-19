/*
 *  Program Name    :  AdminChat
 *  File Name       :  adminchat.cpp
 *  Description     :  관리자용 채팅 프로그램
 *                      -> 채팅 기능
 *                      -> 파일 전송 기능
 *                      -> 공지사항 등록 기능
*/



#include "adminchat.h"
#include "ui_adminchat.h"
#include "delegate.h"

#include <QDataStream>
#include <QTcpSocket>
#include <QApplication>
#include <QThread>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>
#include <QTextEdit>

#include <QApplication>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>


AdminChat::AdminChat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminChat)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    // PushButton connection for sign-in
    connect(ui->signInButton, &QPushButton::clicked,
            [=]{
        adminSocket = new QTcpSocket(this);                                //Create socket

        connect(adminSocket, &QAbstractSocket::errorOccurred,              //Print Debug message about error
                [=]{ qDebug() << adminSocket->errorString(); });
        connect(adminSocket, SIGNAL(readyRead()), SLOT(receiveData()));    //Receive socket from Server
        connect(adminSocket, SIGNAL(disconnected()), SLOT(disconnect()));  //Disconnected, Send message to Server by writting socket
        adminSocket->connectToHost("127.0.0.1", 8000);
        adminSocket->waitForConnected();                   //Wait for few ms until client connects to server
        QString signInData = ui->idLine->text() + "|" + ui->passwordLine->text();
        sendProtocol(Admin_In, signInData.toStdString().data());    //Write socket
    } );

    // For file sending
    fileSocket = new QTcpSocket(this);
    connect(fileSocket, SIGNAL(bytesWritten(qint64)), SLOT(goOnSend(qint64)));
    connect(ui->fileButton, SIGNAL(clicked()), SLOT(sendFile()));

    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();


    QSqlDatabase chatDB = QSqlDatabase::addDatabase("QODBC", "Chat_Admin");
    chatDB.setDatabaseName("Oracle11gx64");
    chatDB.setUserName("chat_admin");
    chatDB.setPassword("admin");
    if (!chatDB.open()) {
        qDebug() << chatDB.lastError().text();
    } else {
        qDebug("Chat_Admin DB connect success");
    }

    customerModel = new QSqlQueryModel(ui->customerTableView);
    noticeModel = new QSqlQueryModel(ui->noticeBoard);

    updateCustomerList();
}

AdminChat::~AdminChat()
{
    delete ui;
    adminSocket->close();
    fileSocket->close();
}

// Send data to server
void AdminChat::sendProtocol(Protocol_Type type, QString msg, int size)
{
    QByteArray dataArray;           // Data for sending
    QDataStream out(&dataArray, QIODevice::WriteOnly);
    out.device()->seek(0);
    out << type;
    out.writeRawData(msg.toStdString().data(), size);
    adminSocket->write(dataArray);     // Send data to server
    adminSocket->flush();
    while(adminSocket->waitForBytesWritten());
}

// Receive the data from server
void AdminChat::receiveData()
{
    QTcpSocket *receiveSocket = dynamic_cast<QTcpSocket *>(sender());
    QByteArray bytearray = receiveSocket->read(1024);

    // Data for sending
    Protocol_Type type;
    char data[1020];
    memset(data, 0, 1020);

    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in.device()->seek(0);
    in >> type;
    in.readRawData(data, 1020);

    switch(type) {
    case Admin_In:
        ui->stackedWidget->setCurrentIndex(1);

        // data = "count || ck1 | ck2 | ck3 | ck4 | ......|"
        if (((QString)data).split("||")[0].toInt() > 0) {
            int count = ((QString)data).split("||")[0].toInt();
            QString ckList = ((QString)data).split("||")[1];

            for (int i = 0; i < count; i++) {
                waitVector.append(ckList.split("|")[i]);
            }

        }
        updateNotice();
        updateCustomerList();
        break;
    case Admin_In_Fail:
        QMessageBox::critical(this, tr("Fail!"), \
                              tr("Please Check your AdminID and Password"));
        break;
    case Sign_In: { //data = "customerkey | name"
        QString ck = ((QString)data).split("|")[0];
        qDebug() << ck;
        waitVector.append(ck);
        updateCustomerList();
        break;
    }
    case In: { //data = customerkey
        waitVector.removeAll(data);
        chattingVector.append(data);
        updateCustomerList();
        chatOpen(data);
        break;
    }
    case Message:
        emit message(data);
        break;
    case Out: {
        int count = ui->chatArea->count();
        for (int i = 0; i < count; i++) {
            if (data == ui->chatArea->tabText(i)) {
                ui->chatArea->removeTab(i);
                break;
            }
        }
        chattingVector.removeAll(data);
        waitVector.append(data);
        updateCustomerList();
        QMessageBox::critical(this, tr("Customer Out!"), \
                              tr("The customer (%1)  has ended the chat.").arg(data));
        break;
    }
    case Close: {
        int count = ui->chatArea->count();
        for (int i = 0; i < count; i++) {
            if (data == ui->chatArea->tabText(i)) {
                ui->chatArea->removeTab(i);
                break;
            }
        }
        chattingVector.removeAll(data);
        waitVector.removeAll(data);
        updateCustomerList();
        QMessageBox::critical(this, tr("Customer Out!"), \
                              tr("The customer (%1) has logged out").arg(data));
        break;
    }
    };
}

// Append new Tab of chat when new admin sign-in
void AdminChat::chatOpen(QString ck)
{
    int index = ui->chatArea->count();

    QWidget *chat = new QWidget(this);
    QTextEdit *message = new QTextEdit(chat);
    QLineEdit *inputLine = new QLineEdit(chat);
    QPushButton *sendButton = new QPushButton(tr("Send"), chat);

    chat->setBaseSize(670, 300);
    message->setGeometry(10, 10, 650, 250);
    inputLine->setGeometry(10, 270, 540, 20);
    sendButton->setGeometry(560, 270, 100, 20);

    message->setReadOnly(true);

    chat->showMaximized();
    ui->chatArea->insertTab(index, chat, ck);
    message->append("<font color=green>***" + ck + "님과 채팅을 시작했습니다.</font>");

    connect(sendButton, &QPushButton::clicked ,
            [=]{
        if (!inputLine->text().length()) return;
        QString tabText = ui->chatArea->tabText(ui->chatArea->currentIndex());
        QString msg = tabText.split("_")[0] + "||" +        //CK_NAME형태(TabTitle)에서 CK만 보내기
                "<font color=orange><b> OSSTEM IMPLANT : </b></font>" + inputLine->text();
        sendProtocol(Message, msg);
        message->append("<font color=orange><b> ME : </b></font>" + inputLine->text());
        inputLine->clear();
        inputLine->setFocus();
    });

    connect(inputLine, &QLineEdit::returnPressed , [=]{
        if (!inputLine->text().length()) return;
        QString tabText = ui->chatArea->tabText(ui->chatArea->currentIndex());
        QString msg = tabText + "||"
                + "<font color=orange><b> OSSTEM IMPLANT : </b></font>" + inputLine->text();
        sendProtocol(Message, msg);
        message->append("<font color=orange><b> ME : </b></font>" + inputLine->text());
        inputLine->clear();
        inputLine->setFocus();
    });

    connect(this, &AdminChat::message, this, [=](QString data){
        if (data.split("|")[0] == ui->chatArea->tabText(index)) {
            message->append("<font color=blue><b> " + data.split("|")[0]
                    + " : </b></font>" + data.split("|")[1]);
        }
    });
}

// Invite Customer
void AdminChat::on_inviteButton_clicked()
{
    QString ck = customerModel->data(ui->customerTableView->currentIndex().siblingAtColumn(0)).toString();
    if (chattingVector.contains(ck)) {
        QMessageBox::critical(this, tr("Chatting!"), \
                              tr("The customer is already chatting"));
        return;
    } else if (!(waitVector.contains(ck))) {
        QMessageBox::critical(this, tr("Not In"), \
                              tr("The customer isn't sign-in yet"));
        return;
    } else if (chattingVector.count() > 2) {
        QMessageBox::critical(this, tr("No More"), \
                              tr("Let's improve the quality of service by serving only two customers"));
        return;
    } else {
        waitVector.removeAll(ck);
        chattingVector.append(ck);
        updateCustomerList();
        sendProtocol(Invite, ck);
        chatOpen(ck);
    }
}

// Kick Out Customer
void AdminChat::on_kickButton_clicked()
{
    QString ck = customerModel->data(ui->customerTableView->currentIndex().siblingAtColumn(0)).toString();
    if (!(chattingVector.contains(ck))) {
        QMessageBox::critical(this, tr("Not Chatting!"), \
                              tr("The customer isn't chatting"));
        return;
    }

    chattingVector.removeAll(ck);
    waitVector.append(ck);
    updateCustomerList();
    sendProtocol(Kick_Out, ck);

    int count = ui->chatArea->count();
    for (int i = 0; i < count; i++) {
        if (ck == ui->chatArea->tabText(i))
            ui->chatArea->removeTab(i);
    }

}

// Seperate file for sending
void AdminChat::goOnSend(qint64 numBytes) // Start sending file content
{
    byteToWrite -= numBytes; // Remaining data size
    outBlock = file->read(qMin(byteToWrite, numBytes));
    fileSocket->write(outBlock);

    progressDialog->setMaximum(totalSize);
    progressDialog->setValue(totalSize-byteToWrite);

    if (byteToWrite == 0) { // Send completed
        qDebug("File sending completed!");
        progressDialog->reset();
    }
}

// Append new notice
void AdminChat::on_noticeButton_clicked()
{
    if (!ui->noticeInput->text().length()) return;
    insertNotice();
    updateNotice();
    ui->noticeInput->clear();
}

void AdminChat::sendFile()
{
    loadSize = 0;
    byteToWrite = 0;
    totalSize = 0;
    outBlock.clear();

    QString filename = QFileDialog::getOpenFileName(this);
    if(filename.length()) {
        file = new QFile(filename);
        file->open(QFile::ReadOnly);

        qDebug() << QString("file %1 is opened").arg(filename);
        progressDialog->setValue(0); // first time, any data send

        if (!isSent) { // Check the connection to server, first time
            fileSocket->connectToHost("127.0.0.1", 8001);
            isSent = true;
        }

        byteToWrite = totalSize = file->size(); // Data remained yet
        loadSize = 1024; // Size of data per a block

        QDataStream out(&outBlock, QIODevice::WriteOnly);
        out << qint64(0) << qint64(0) << filename << (QString)"Admin";

        totalSize += outBlock.size();
        byteToWrite += outBlock.size();

        out.device()->seek(0);
        out << totalSize << qint64(outBlock.size());

        fileSocket->write(outBlock);

        progressDialog->setMaximum(totalSize);
        progressDialog->setValue(totalSize-byteToWrite);
        progressDialog->show();
    }
    qDebug() << QString("Sending file %1").arg(filename);
}

void AdminChat::updateNotice()
{
    QSqlDatabase chatDB = QSqlDatabase::database("Chat_Admin");
    noticeModel->setQuery("SELECT NOTICE_CONTENTS FROM sys.NOTICE_TABLE ORDER BY NOTICE_DATE", chatDB);
    noticeModel->setHeaderData(0, Qt::Horizontal, tr("CONTENTS"));

    ui->noticeBoard->setModel(noticeModel);
    ui->noticeBoard->horizontalHeader()->setStretchLastSection(true);
    ui->noticeBoard->horizontalHeader()->setStyleSheet(
                "QHeaderView { font-size: 10pt; color: blue; }");
}

void AdminChat::insertNotice()
{
    QSqlDatabase chatDB = QSqlDatabase::database("Chat_Admin");
    QSqlQuery input(chatDB);
    input.prepare("INSERT INTO sys.NOTICE_TABLE(NOTICE_DATE, NOTICE_CONTENTS) VALUES (:date, :notice)");
    input.bindValue(":date", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    input.bindValue(":notice", ui->noticeInput->text());
    input.exec();

    sendProtocol(Notice, "notice");
}

void AdminChat::closeEvent(QCloseEvent*)
{
    disconnect();
}

void AdminChat::disconnect()
{
    sendProtocol(Close, ui->idLine->text().toStdString().data());
    while(adminSocket->waitForDisconnected(1000)) {
        QThread::usleep(10);
    }
    adminSocket->disconnectFromHost();
    adminSocket->deleteLater();
}


void AdminChat::updateCustomerList()
{
    QSqlDatabase chatAdminDB = QSqlDatabase::database("Chat_Admin");
    customerModel->setQuery("SELECT CUSTOMER_KEY, CLINIC_NAME FROM sys.CUSTOMER_TABLE ORDER BY 1", chatAdminDB);
    customerModel->setHeaderData(0, Qt::Horizontal, tr("CUSTOMER_KEY"));
    customerModel->setHeaderData(1, Qt::Horizontal, tr("CLINIC"));

    ui->customerTableView->setModel(customerModel);
    ui->customerTableView->horizontalHeader()->setStretchLastSection(true);
    ui->customerTableView->horizontalHeader()->setStyleSheet(
                "QHeaderView { font-size: 10pt; color: blue; }");

    QVector<int> tmpWaitVector;
    QVector<int> tmpChattingVector;

    foreach (auto waitCk, waitVector) {
        QModelIndexList indexes = customerModel->match(customerModel->index(0, 0), Qt::EditRole, waitCk, -1, Qt::MatchFlags(Qt::MatchExactly));
        foreach (auto idx, indexes) {
            tmpWaitVector.append(idx.row());
        }
    }
    foreach (auto chatCk, chattingVector) {
        QModelIndexList indexes = customerModel->match(customerModel->index(0, 0), Qt::EditRole, chatCk, -1, Qt::MatchFlags(Qt::MatchExactly));
        foreach (auto idx, indexes) {
            tmpChattingVector.append(idx.row());
        }
    }

    Delegate *delegate = new Delegate(ui->customerTableView);
    delegate->setWaitVector(tmpWaitVector);
    delegate->setChattingVector(tmpChattingVector);
    ui->customerTableView->setItemDelegateForColumn(0, delegate);
    ui->customerTableView->setItemDelegateForColumn(1, delegate);
}
