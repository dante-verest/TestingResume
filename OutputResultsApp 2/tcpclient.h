#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QSettings>
#include <QStringList>
#include <QModelIndex>

QT_BEGIN_NAMESPACE
class QListView;
class QStringListModel;
class QTcpSocket;
class QSettings;
class QAbstractSocket;
class QModelIndex;
QT_END_NAMESPACE

class TcpClient : public QMainWindow
{
    Q_OBJECT
private:
    QSettings m_settings;
    QStringList m_stringList;
    QStringListModel* m_pStringListModel;
    QListView* m_pListView;
    QTcpSocket* m_pClientSocket;
    quint16 m_uNextBlockSize;

public:
    TcpClient(const QString& asHost, int anHost, QWidget *parent = nullptr);
    ~TcpClient();
    void readSettings();
    void writeSettings();

private:
    void addWidgets();
    void createTcpConnection(const QString& sHost, int nHost);
    void createActions();
    void addString(const QString& str);

private slots:
    void openAbout();
    void errorConnection(QAbstractSocket::SocketError);
    void connectedToServer();
    void readyReadServer();
    void sendToServer(const QString& str);
    void showSelectionItem(QModelIndex);
};
#endif // TCPCLIENT_H
