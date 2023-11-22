#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QMainWindow>
#include <QTime>
#include <QSettings>

QT_BEGIN_NAMESPACE
class QTextEdit;
class QSettings;
class QUdpSocket;
class QTime;
class QTimer;
QT_END_NAMESPACE

class UdpClient : public QMainWindow
{
    Q_OBJECT
private:
    QTextEdit* m_pTextEdit;
    QUdpSocket* m_pUdpSocket;
    QSettings m_settings;
    QTimer* m_pTimer;

public:
    explicit UdpClient(QMainWindow *parent = nullptr);
    virtual ~UdpClient();
    void readSettings();
    void writeSettings();

private slots:
    void sendDatagram();
    void startSendDatagram();
    void stopSendDatagram();
    void openAbout();

private:
    void sendSpecialDatagram(const QString& str);
    void createActions();
    void addWidgets();
};
#endif // UDPCLIENT_H
