#ifndef SERVER_H
#define SERVER_H

#include <QThread>

class ThreadStack;
class FromUdpClient;
class ToTcpClient;

class Server : public QObject
{
    Q_OBJECT
private:
    ThreadStack* m_pThreadStack;
    FromUdpClient* m_pFromUdpClient;
    ToTcpClient* m_pToTcpClient;
    QThread m_thread_1;
    QThread m_thread_2;

public:
    Server(int anPort = 0, QObject* parent = nullptr);
    ~Server();
};

#endif // SERVER_H
