#include "totcpclient.h"
#include "threadstack.h"

#include <iostream>
#include <chrono>
#include <QCoreApplication>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDataStream>
#include <QTime>
#include <QThread>

ToTcpClient::ToTcpClient(ThreadStack *&aStack, int anPort, QObject *parent)
    : ReadingClass(parent)
    , m_pStack(aStack)
{
    m_pClientSocket = new QTcpSocket;
    m_bStop = false;

    createTcpServer(anPort);
}

void ToTcpClient::run()
{
    while(!m_bStop)
    {
        QCoreApplication::processEvents();
        QThread::msleep(15);
    }
}

void ToTcpClient::setStop()
{
    m_bStop = true;
}

void ToTcpClient::createTcpServer(int anPort)
{
    m_pTcpServer = new QTcpServer(this);
    if(!m_pTcpServer->listen(QHostAddress::Any, anPort))
    {
        std::cout << "Server error: unable to start the server! Error " <<
                     m_pTcpServer->errorString().toStdString() << "." << std::endl;
        m_pTcpServer->close();
        return;
    }
    connect(m_pTcpServer, &QTcpServer::newConnection, this, &ToTcpClient::newTcpConnection);
}

void ToTcpClient::sendToClient(QTcpSocket* pSocket, const QString& str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);
    out << quint16(0) << QTime::currentTime() << str;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    pSocket->write(arrBlock);
}

void ToTcpClient::newTcpConnection()
{
    m_pClientSocket = m_pTcpServer->nextPendingConnection();
    /*
     * если мы закроем третье приложение (OutputRsultApp 2) и откроем его снова,
     * не закрывая при этом другие приложения, то необходимо освободить старый сокет
     * и создать новый, иначе segmentation fault
     */
    connect(m_pClientSocket, &QTcpSocket::disconnected, this, [this](){
        m_pClientSocket->deleteLater();
        m_pClientSocket = new QTcpSocket;
    });
    connect(m_pClientSocket, &QTcpSocket::readyRead, this, &ToTcpClient::readClient);
    sendToClient(m_pClientSocket, " Server response: connected!");
    std::cout << "Current ToTcpClient thread ID: " <<
                 reinterpret_cast<long long>(QThread::currentThreadId()) << "." << std::endl;
}

void ToTcpClient::readClient()
{
    QDataStream in(m_pClientSocket);
    in.setVersion(QDataStream::Qt_5_3);
    while(true)
    {
        if(!m_uNextBlockSize)
        {
            if(m_pClientSocket->bytesAvailable() < sizeof(quint16))
                break;
            in >> m_uNextBlockSize;
        }
        if(m_pClientSocket->bytesAvailable() < m_uNextBlockSize)
            break;

        QTime time;
        QString str;
        in >> time >> str;
        std::cout << time.toString("hh:mm:ss.zzz").toStdString() <<
                     " Client has sent - " << str.toStdString() << "." << std::endl;
        m_uNextBlockSize = 0;
        sendToClient(m_pClientSocket, "Server has connection to client!");
    }
}

void ToTcpClient::dataToTcpClient()
{
    if(m_pClientSocket->isValid())
    {
        auto start = std::chrono::steady_clock::now();
        int n = m_pStack->pop();
        auto finish = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
        std::cout << "\tReading data \'" << n << "\' by stream took " << elapsed << " nanoseconds." << std::endl;
        sendToClient(m_pClientSocket, QString::number(n));
    }
}
