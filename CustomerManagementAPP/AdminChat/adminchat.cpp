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

    noticeModel = new QSqlQueryModel(ui->noticeBoard);
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
    if (receiveSocket->bytesAvailable() > 1024) return;
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
        updateNotice();
        break;
    case Admin_In_Fail:
        QMessageBox::critical(this, tr("Fail!"), \
                              tr("Please Check your AdminID and Password"));
        break;
    case Sign_In: {
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, ((QString)data).split("|")[0]);
        item->setText(1, ((QString)data).split("|")[1]);
        item->setText(2, "Connected");
        ui->customerTreeWidget->addTopLevelItem(item);
        break;
    }
    case In:
        chatOpen((QString)data);
        break;
    case Message:
        emit message((QString)data);
        break;
    case Out:
        QMessageBox::critical(this, tr("Customer Out!"), \
                              tr("The customer (%1)  has ended the chat.").arg(data));
        ui->chatArea->removeTab(chatIndexHash.key(data));
        chatIndexHash.remove(chatIndexHash.key(data));
        foreach (auto item, ui->customerTreeWidget->findItems(data, Qt::MatchExactly, 0)) {
            item->setText(2, "Connected");
        }
        break;
    case Close:
        foreach (auto item, ui->customerTreeWidget->findItems(data, Qt::MatchExactly, 0)) {
            ui->customerTreeWidget->takeTopLevelItem(ui->customerTreeWidget->indexOfTopLevelItem(item));
        }
        if (chatIndexHash.contains(chatIndexHash.key(data))) {
            ui->chatArea->removeTab(chatIndexHash.key(data));
            chatIndexHash.remove(chatIndexHash.key(data));
        }
        QMessageBox::critical(this, tr("Customer Out!"), \
                              tr("The customer (%1) has logged out").arg(data));
        break;
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
//    chatIndexHash[index] = ckName.split("_")[0];
    chatIndexHash[index] = ck;
    message->append("<font color=green>***" + ck + "님과 채팅을 시작했습니다.</font>");
    foreach (auto item, ui->customerTreeWidget->findItems(ck, Qt::MatchExactly, 0)) {
        item->setText(2, "Chatting...");
    }

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
        QString msg = tabText.split("_")[0] + "||"           //CK_NAME형태(TabTitle)에서 CK만 보내기
                + "<font color=orange><b> OSSTEM IMPLANT : </b></font>" + inputLine->text();
        sendProtocol(Message, msg);
        message->append("<font color=orange><b> ME : </b></font>" + inputLine->text());
        inputLine->clear();
        inputLine->setFocus();
    });

    connect(this, &AdminChat::message, this, [=](QString data){
        if (index == chatIndexHash.key(data.split("|")[0]))
            message->append("<font color=blue><b> " + data.split("|")[0]
                    + " : </b></font>" + data.split("|")[1]);
    });
}

// Invite Customer
void AdminChat::on_inviteButton_clicked()
{
    if (ui->chatArea->count() > 2) {
        QMessageBox::critical(this, tr("No More"), \
                              tr("Let's improve the quality of service by serving only two customers"));
        return;
    }
    foreach (auto item, ui->customerTreeWidget->selectedItems()) {
        if (item->text(2) == "Chatting...") {
            QMessageBox::critical(this, tr("Chatting!"), \
                                  (tr("The customer (%1) is already chatting")).arg(item->text(0)));
            return;
        } else {
            sendProtocol(Invite, item->text(0));
            chatOpen(item->text(0) + "_" + item->text(1));
        }
    }
}

// Kick Out Customer
void AdminChat::on_kickButton_clicked()
{
    foreach (auto item, ui->customerTreeWidget->selectedItems()) {
        if (item->text(2) == "Connected") {
            QMessageBox::critical(this, tr("Not Be chatting!"), \
                                  (tr("The customer (%1) is not chatting")).arg(item->text(0)));
            return;
        } else {
            sendProtocol(Kick_Out, item->text(0));
            item->setText(2, "Connected");
            ui->chatArea->removeTab(chatIndexHash.key(item->text(0)));
            chatIndexHash.remove(chatIndexHash.key(item->text(0)));
        }
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
        out << qint64(0) << qint64(0) << filename << ui->idLine->text();

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
    noticeModel->setQuery("SELECT * FROM sys.NOTICE_TABLE ORDER BY NOTICE_DATE", chatDB);
    noticeModel->setHeaderData(0, Qt::Horizontal, tr("DATE"));
    noticeModel->setHeaderData(1, Qt::Horizontal, tr("NOTICE"));

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
    input.bindValue(":date", QDate::currentDate());
    input.bindValue(":notice", ui->noticeInput->text());
    input.exec();

    sendProtocol(Notice, "notice");
}
