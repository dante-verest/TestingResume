#include "fromudpclient.h"
#include "threadstack.h"

#include <iostream>
#include <chrono>
#include <QCoreApplication>
#include <QUdpSocket>
#include <QTimer>
#include <QDataStream>
#include <QTime>
#include <QThread>

FromUdpClient::FromUdpClient(ThreadStack *&aStack, QObject *parent)
    : WritingClass(parent)
    , m_pStack(aStack)
{
    m_pTimer = new QTimer;
    m_bTimer = false;
    m_bStop = false;

    connect(m_pTimer, &QTimer::timeout, this, &FromUdpClient::timeToSendSignalForReading);

    m_pTimer->start(1000);

    connectToUdpSocket();
}

void FromUdpClient::run()
{
    while(!m_bStop)
    {
        /* небольшая задержка потока необходиам для разгрузки процессора, также
        * она даёт время для отображения информации в терминале
        * (без неё информация о текущей датаграмме не успевала полностью вывестись
        * и выводилась следующая информация)
        */
        QThread::msleep(15);
        if(!m_pStack->isEmpty())
        {
            QCoreApplication::processEvents();
            /*
             * объекты таймера позволяют не тормозить выполнение программы из-за одного потока,
             * но обеспечивают необходимую для работы задежку
             */
            if(m_bTimer)
            {
                emit dataTransmission();
                m_bTimer = false;
            }
        }
    }
}

void FromUdpClient::setStop()
{
    m_bStop = true;
}

void FromUdpClient::connectToUdpSocket()
{
    m_pUdpSocket = new QUdpSocket;
    m_pUdpSocket->bind(1236);
    // здесь потребовалось явно указать тип соединения Qt::DirectConnection,
    // иначе m_pUdpSocket будет невиден для EnteringSettingsApp 2
    connect(m_pUdpSocket, &QUdpSocket::readyRead, this, &FromUdpClient::readClient, Qt::DirectConnection);
    std::cout << "Current FromUdpClient thread ID: "  <<
                 reinterpret_cast<long long>(QThread::currentThreadId()) << "." << std::endl;
}

void FromUdpClient::readClient()
{
    QByteArray baDatagrams;
    do
    {
        baDatagrams.resize(m_pUdpSocket->pendingDatagramSize());
        m_pUdpSocket->readDatagram(baDatagrams.data(), baDatagrams.size());
    } while(m_pUdpSocket->hasPendingDatagrams());

    QString str;
    QDataStream in(&baDatagrams, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_3);
    in >> str;
    bool bIsInteger = false;
    int n = str.toInt(&bIsInteger);
    if(bIsInteger)
    {
        auto start = std::chrono::steady_clock::now();
        m_pStack->push(n);
        auto finish = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
        std::cout << QTime::currentTime().toString("hh:mm:ss.zzz").toStdString() << " " << n << std::endl;
        std::cout << "\tWriting data \'" << n << "\' by stream took " << elapsed << " nanoseconds." << std::endl;
    }
    else
    {
        std::cout << QTime::currentTime().toString("hh:mm:ss.zzz").toStdString() << " " << str.toStdString() << std::endl;
    }
}

void FromUdpClient::timeToSendSignalForReading()
{
    m_bTimer = true;
}
