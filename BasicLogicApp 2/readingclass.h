#ifndef READINGCLASS_H
#define READINGCLASS_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QTcpSocket;
QT_END_NAMESPACE

class ReadingClass : public QObject
{
    Q_OBJECT
public:
    explicit ReadingClass(QObject *parent = nullptr);
    ~ReadingClass();
    virtual void run() = 0;
    virtual void setStop() = 0;

private:
    virtual void sendToClient(QTcpSocket* pSocket, const QString& str) = 0;

public slots:
    virtual void readClient() = 0;

};

#endif // READINGCLASS_H
