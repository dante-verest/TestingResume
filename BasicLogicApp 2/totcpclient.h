#ifndef TOTCPCLIENT_H
#define TOTCPCLIENT_H

#include "readingclass.h"

QT_BEGIN_NAMESPACE
class QTcpSocket;
class QTcpServer;
QT_END_NAMESPACE
class ThreadStack;

class ToTcpClient : public ReadingClass
{
    Q_OBJECT
private:
    QTcpSocket* m_pClientSocket;
    QTcpServer* m_pTcpServer;
    quint16 m_uNextBlockSize;
    ThreadStack* m_pStack;
    bool m_bStop;

public:
    explicit ToTcpClient(ThreadStack *&aStack, int anPort, QObject *parent = nullptr);
    void run() override;
    void setStop() override;

private:
    void createTcpServer(int anPort);
    void sendToClient(QTcpSocket* pSocket, const QString& str) override;

public slots:
    virtual void newTcpConnection();
    void readClient() override;
    void dataToTcpClient();

};

#endif // TOTCPCLIENT_H
