#include "tcpclient.h"

#include <QWidget>
#include <QListView>
#include <QStringListModel>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTime>

TcpClient::TcpClient(const QString &asHost, int anHost, QWidget *parent)
    : QMainWindow(parent)
    , m_uNextBlockSize(0)
{
    createActions();
    addWidgets();
    createTcpConnection(asHost, anHost);
}

TcpClient::~TcpClient()
{
    writeSettings();
}

void TcpClient::readSettings()
{
    m_settings.beginGroup("/Settings");
    int nX = m_settings.value("/x", x()).toInt();
    int nY = m_settings.value("/y", y()).toInt();
    int nHeight = m_settings.value("/height", height()).toInt();
    int nWidth = m_settings.value("/width", width()).toInt();
    m_settings.endGroup();

    move(nX, nY);
    resize(nWidth, nHeight);
}

void TcpClient::writeSettings()
{
    m_settings.beginGroup("/Settings");
    m_settings.setValue("/x", x());
    m_settings.setValue("/y", y());
    m_settings.setValue("/height", height());
    m_settings.setValue("/width", width());
    m_settings.endGroup();
}

void TcpClient::openAbout()
{
    QMessageBox::about(this,
                       tr("About OutputResultsApp"),
                       tr("This  program outputs incoming data from\n"
                          "BasicLogicApp to QListView window (class).\n"
                          "You can double click on any line for getting\n"
                          "message box that will display the line itself.\n"
                          "The incoming data is integer numbers or\n"
                          "information messages from server."));
}

void TcpClient::errorConnection(QAbstractSocket::SocketError err)
{
    QString sError = " Error: " + (err == QAbstractSocket::HostNotFoundError ?
                                    "The host was not found!" :
                                    err == QAbstractSocket::RemoteHostClosedError ?
                                        "The remote socked is closed!" :
                                        err == QAbstractSocket::ConnectionRefusedError ?
                                            "The connection was refused!" :
                                            QString(m_pClientSocket->errorString()));
    addString(QTime::currentTime().toString("hh:mm:ss.zzz") + sError);
}

void TcpClient::connectedToServer()
{
    addString(QTime::currentTime().toString("hh:mm:ss.zzz") + " Received the connect() signal!");
}

void TcpClient::readyReadServer()
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

        addString(time.toString("hh:mm:ss.zzz") + " " + str);

        m_uNextBlockSize = 0;
    }
}

void TcpClient::sendToServer(const QString& str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);
    out << quint16(0) << QTime::currentTime() << str;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    m_pClientSocket->write(arrBlock);
}

void TcpClient::showSelectionItem(QModelIndex index)
{
    if(!index.isValid())
        return;
    QMessageBox::information(this, "Current index data", index.data().toString() + ".");
}

void TcpClient::createActions()
{
    QMenu* pHelpMenuPoint = menuBar()->addMenu(tr("H&elp"));
    QAction* pAboutPoint = pHelpMenuPoint->addAction(tr("&About..."), this, &TcpClient::openAbout);
    pAboutPoint->setShortcut(tr("Ctrl+F"));
    QAction* pExitPoint = pHelpMenuPoint->addAction(tr("&Exit"), this, &QWidget::close);
    pExitPoint->setShortcut(QKeySequence::Close);
}

void TcpClient::addWidgets()
{
    QWidget* pWidgets = new QWidget;

    m_pListView = new QListView;
    m_pStringListModel = new QStringListModel;
    m_pListView->setModel(m_pStringListModel);
    m_pListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pListView->setWordWrap(true);
    connect(m_pListView, &QListView::doubleClicked, this, &TcpClient::showSelectionItem);

    readSettings();

    QVBoxLayout* pVBoxLayout = new QVBoxLayout;
    pVBoxLayout->setContentsMargins(5, 5, 5, 5);
    pVBoxLayout->addWidget(m_pListView);
    pWidgets->setLayout(pVBoxLayout);
    setCentralWidget(pWidgets);
}

void TcpClient::createTcpConnection(const QString &sHost, int nHost)
{
    m_pClientSocket = new QTcpSocket;
    m_pClientSocket->connectToHost(sHost, nHost);
    connect(m_pClientSocket, &QTcpSocket::connected, this, &TcpClient::connectedToServer);
    connect(m_pClientSocket, &QTcpSocket::readyRead, this, &TcpClient::readyReadServer);
    connect(m_pClientSocket, static_cast<void (QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
            this, &TcpClient::errorConnection);
    sendToServer("Connect to client!");
}

void TcpClient::addString(const QString &str)
{
    m_stringList << str;
    m_pStringListModel->setStringList(m_stringList);
    int nRow = m_pStringListModel->rowCount();
    m_pStringListModel->insertRows(nRow, 1);
    QModelIndex index = m_pStringListModel->index(nRow);
    m_pListView->setCurrentIndex(index);
    m_pListView->edit(index);
}
