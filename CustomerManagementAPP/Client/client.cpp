/*
 *  Program Name    :  Client
 *  File Name       :  client.cpp
 *  Description     :  고객용 채팅 프로그램
 *                      -> 채팅 기능
 *                      -> 파일 전송 기능
*/


#include "client.h"
#include "ui_client.h"

#include <QRegularExpression>
#include <QRegularExpressionValidator>
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

Client::Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Client)
{
    ui->setupUi(this);
    setFixedSize(450, 335);
    ui->stackedWidget->showMaximized();
    ui->stackedWidget->setCurrentIndex(0);
    noticeLoad();

// 1. Sign_In & Chat_In
    // 1-1. Set Widgets for inputting Server-info
    ui->serverAddress->setText("127.0.0.1");
    QRegularExpression re("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$|");
    QRegularExpressionValidator validator(re);
    ui->serverAddress->setValidator(&validator);
    ui->serverPort->setText("8000");
    ui->serverPort->setInputMask("00000;_");

    // 1-2. Sign_In
    connect(ui->signInButton, &QPushButton::clicked,
            [=]{
        clientSocket = new QTcpSocket(this);                                //Create socket

        connect(clientSocket, &QAbstractSocket::errorOccurred,              //Print Debug message about error
                [=]{ qDebug() << clientSocket->errorString(); });
        connect(clientSocket, SIGNAL(readyRead()), SLOT(receiveData()));    //Receive socket from Server
        clientSocket->connectToHost(ui->serverAddress->text(),              //Connect to Server
                                    ui->serverPort->text().toInt());
        clientSocket->waitForConnected();                   //Wait for few ms until client connects to server
        QString signInData = ui->customerKey->text() + "|" + ui->clinic->text();
        sendProtocol(Sign_In, signInData.toStdString().data());    //Write socket
    } );

    // 1-3. Chat_In
    connect(ui->chatButton, &QPushButton::clicked,
            [=]{
        if (ui->chatButton->text() == "Chat In") {
            sendProtocol(In, ui->customerKey->text().toStdString().data());
        } else if (ui->chatButton->text() == "Chat Out") {
            sendProtocol(Out, ui->customerKey->text().toStdString().data());
            ui->message->append("*********채팅 종료*********");
            ui->chatButton->setText(tr("Chat In"));
            ui->sendButton->setEnabled(false);
            ui->fileButton->setEnabled(false);
            ui->inputLine->setEnabled(false);
            ui->message->setEnabled(false);
        }
    } );

    // Connection for sending data to server
    connect(ui->inputLine, SIGNAL(returnPressed()), SLOT(sendData()));
    connect(ui->sendButton, SIGNAL(clicked()), SLOT(sendData()));
    connect(ui->fileButton, SIGNAL(clicked()), SLOT(sendFile()));

    ui->inputLine->setEnabled(false);
    ui->sendButton->setEnabled(false);
    ui->fileButton->setDisabled(true);

    // For file sending
    fileSocket = new QTcpSocket(this);
    connect(fileSocket, SIGNAL(bytesWritten(qint64)), SLOT(goOnSend(qint64)));

    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();
}

Client::~Client()
{
    clientSocket->close();
    fileSocket->close();
}

// Send data to server
void Client::sendProtocol(Protocol_Type type, QString data, int size)
{
    QByteArray dataArray;           // Data for sending
    QDataStream out(&dataArray, QIODevice::WriteOnly);
    out.device()->seek(0);
    out << type;
    out.writeRawData(data.toStdString().data(), size);
    clientSocket->write(dataArray);     // Send data to server
    clientSocket->flush();
    while(clientSocket->waitForBytesWritten());
}

// Receive the data from server
void Client::receiveData( )
{
    QTcpSocket *clientSocket = dynamic_cast<QTcpSocket *>(sender());
    if (clientSocket->bytesAvailable() > 1024) return;
    QByteArray bytearray = clientSocket->read(1024);

    // Data for sending
    Protocol_Type type;
    char data[1020];
    memset(data, 0, 1020);

    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in.device()->seek(0);
    in >> type;
    in.readRawData(data, 1020);

    switch(type) {
    case Sign_In:
        ui->stackedWidget->setCurrentIndex(1);
        setWindowTitle(((QString)data).replace("|", "___"));
        break;
    case Sign_In_Fail:
        QMessageBox::critical(this, tr("Fail!"), \
                              tr("Please Check your CustomerKey and Clinic"));
        break;
    case In:
        ui->chatButton->setText(tr("Chat Out"));
        ui->sendButton->setEnabled(true);
        ui->fileButton->setEnabled(true);
        ui->inputLine->setEnabled(true);
        ui->message->setEnabled(true);
        ui->message->setReadOnly(true);
        ui->message->append(QString(data));
        break;
    case In_Fail:
        QMessageBox::critical(this, tr("Chat In Fail!"), \
                              tr("Sorry, All admins are busy"));
        ui->message->append(QString(data));
        break;
    case Message:
        ui->message->append(QString(data));
        break;
    case Invite:
        QMessageBox::critical(this, tr("Invite"), \
                              tr("Invited from Server"));
        ui->chatButton->setText(tr("Chat Out"));
        ui->message->append(QString(data));
        ui->message->setEnabled(true);
        ui->inputLine->setEnabled(true);
        ui->sendButton->setEnabled(true);
        ui->fileButton->setEnabled(true);
        ui->message->setReadOnly(true);
        break;
    case Kick_Out:
        QMessageBox::critical(this, tr("Kick Out"), \
                              tr("Kick out from Server"));
        ui->chatButton->setText(tr("Chat In"));
        ui->message->append("*********채팅 종료*********");
        ui->message->setEnabled(false);
        ui->inputLine->setEnabled(false);
        ui->sendButton->setEnabled(false);
        ui->fileButton->setEnabled(false);
        break;
    case Notice:
        ui->noticeBoard->append(data);
    };
}

// Setting the data when send it to server
void Client::sendData()
{
    QString msg = ui->customerKey->text() + "|" + ui->inputLine->text();
    if (!msg.length()) return;
    ui->message->append("<font color=blue> ME : </font>" + msg.split("|")[1]);              // 보낸메세지 화면표시
    sendProtocol(Message, msg);
    ui->inputLine->clear();
    ui->inputLine->setFocus();
}

// Seperate file for sending
void Client::goOnSend(qint64 numBytes) // Start sending file content
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

// File sending
void Client::sendFile()
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
            fileSocket->connectToHost(ui->serverAddress->text( ),
                                      ui->serverPort->text( ).toInt( ) + 1);
            isSent = true;
        }

        byteToWrite = totalSize = file->size(); // Data remained yet
        loadSize = 1024; // Size of data per a block

        QDataStream out(&outBlock, QIODevice::WriteOnly);
        out << qint64(0) << qint64(0) << filename << ui->customerKey->text();

        totalSize += outBlock.size();
        byteToWrite += outBlock.size();

        out.device()->seek(0);
        out << totalSize << qint64(outBlock.size());

        fileSocket->write(outBlock); // Send the read file to the socket

        progressDialog->setMaximum(totalSize);
        progressDialog->setValue(totalSize-byteToWrite);
        progressDialog->show();
    }
    qDebug() << QString("Sending file %1").arg(filename);
}

// Load previous notice
void Client::noticeLoad()
{
    QFile file("../Admin/data/manage/notice.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QByteArray notice = file.readAll();
    ui->noticeBoard->setPlainText(notice);

    file.close( );
}

// If Client Programs are closed, send data to server
void Client::closeEvent(QCloseEvent*)
{
    sendProtocol(Close, ui->customerKey->text().toStdString().data());
    while(clientSocket->waitForDisconnected(1000))
        QThread::usleep(10);
    clientSocket->disconnectFromHost();
}
