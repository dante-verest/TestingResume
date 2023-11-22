#ifndef FROMUDPCLIENT_H
#define FROMUDPCLIENT_H

#include "writingclass.h"

QT_BEGIN_NAMESPACE
class QUdpSocket;
class QTimer;
QT_END_NAMESPACE
class ThreadStack;

class FromUdpClient : public WritingClass
{
    Q_OBJECT
private:
    QUdpSocket* m_pUdpSocket;
    ThreadStack* m_pStack;
    QTimer* m_pTimer;
    bool m_bTimer;
    bool m_bStop;

public:
    explicit FromUdpClient(ThreadStack *&aStack, QObject *parent = nullptr);
    void run() override;
    void setStop() override;

private:
    void connectToUdpSocket();

signals:
    void dataTransmission();

private slots:
    void readClient() override;
    void timeToSendSignalForReading();
};

#endif // FROMUDPCLIENT_H
