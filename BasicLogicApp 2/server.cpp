#include "server.h"
#include "threadstack.h"
#include "fromudpclient.h"
#include "totcpclient.h"

#include <iostream>

Server::Server(int anPort, QObject *parent)
    : QObject(parent)
{
    m_pThreadStack = new ThreadStack;
    m_pFromUdpClient = new FromUdpClient(m_pThreadStack);
    m_pToTcpClient = new ToTcpClient(m_pThreadStack, anPort);

    connect(&m_thread_1, &QThread::started, m_pFromUdpClient, &FromUdpClient::run);
    connect(&m_thread_2, &QThread::started, m_pToTcpClient, &ToTcpClient::run);

    connect(m_pFromUdpClient, &FromUdpClient::dataTransmission,
            m_pToTcpClient, &ToTcpClient::dataToTcpClient, Qt::QueuedConnection);

    m_pFromUdpClient->moveToThread(&m_thread_1);
    m_pToTcpClient->moveToThread(&m_thread_2);

    m_thread_1.start();
    m_thread_2.start();
}

Server::~Server()
{
    m_pFromUdpClient->setStop();
    m_thread_1.wait();
    m_thread_1.deleteLater();
    m_pToTcpClient->setStop();
    m_thread_2.wait();
    m_thread_2.deleteLater();
}
