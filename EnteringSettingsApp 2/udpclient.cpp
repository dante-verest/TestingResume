#include "udpclient.h"

#include <QWidget>
#include <QUdpSocket>
#include <QTextEdit>
#include <QBoxLayout>
#include <QPushButton>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QTimer>

UdpClient::UdpClient(QMainWindow *parent)
    : QMainWindow(parent)
{
    m_pUdpSocket = new QUdpSocket;
    m_pTimer = new QTimer;

    createActions();
    addWidgets();
}

UdpClient::~UdpClient()
{
    writeSettings();
}

void UdpClient::readSettings()
{
    m_settings.beginGroup("/Settings");
    QString sText = m_settings.value("/text", "").toString();
    int nX = m_settings.value("/x", x()).toInt();
    int nY = m_settings.value("/y", y()).toInt();
    int nHeight = m_settings.value("/height", height()).toInt();
    int nWidth = m_settings.value("/width", width()).toInt();
    m_settings.endGroup();

    m_pTextEdit->setPlainText(sText);
    move(nX, nY);
    resize(nWidth, nHeight);
}

void UdpClient::writeSettings()
{
    m_settings.beginGroup("/Settings");
    m_settings.setValue("/text", m_pTextEdit->toPlainText());
    m_settings.setValue("/x", x());
    m_settings.setValue("/y", y());
    m_settings.setValue("/height", height());
    m_settings.setValue("/width", width());
    m_settings.endGroup();
}

void UdpClient::sendSpecialDatagram(const QString &str)
{
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);
    out << str;
    m_pUdpSocket->writeDatagram(baDatagram, QHostAddress::LocalHost, 1236);
}

void UdpClient::sendDatagram()
{
    const QString sEmptyTextEdit = "Write at least one integer number!";
    const QString sErrorData = "It is not a number or integer!";
    bool bIsInteger = false;
    QStringList aStrings = m_pTextEdit->toPlainText().split('\n');
    QByteArray baDatagrams;
    QDataStream out(&baDatagrams, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);
    // проверку корректности ввода никто не отменял
    if(aStrings.at(0).compare("") != 0 ||
            (aStrings.at(0).compare("") == 0 && aStrings.size() > 1))
    {
        aStrings.at(0).toInt(&bIsInteger);
        if(aStrings.at(0).compare(sErrorData) == 0 ||
                aStrings.at(0).compare(sEmptyTextEdit) == 0)
        {
            aStrings.erase(aStrings.begin());
            m_pTextEdit->setPlainText(aStrings.join('\n'));
        }
        else if(!bIsInteger)
        {
            aStrings.insert(0, sErrorData);
            aStrings.erase(aStrings.begin() + 1);
            m_pTextEdit->setPlainText(aStrings.join('\n'));
        }
        else
        {
            out << aStrings.at(0);
            aStrings.erase(aStrings.begin());
            m_pTextEdit->setPlainText(aStrings.join('\n'));
            m_pUdpSocket->writeDatagram(baDatagrams, QHostAddress::LocalHost, 1236);
        }
    }
    else
    {
        m_pTextEdit->append(sEmptyTextEdit);
        stopSendDatagram();
    }
}

void UdpClient::startSendDatagram()
{
    m_pTimer->start(1000);
    connect(m_pTimer, &QTimer::timeout, this, &UdpClient::sendDatagram);
    sendSpecialDatagram("Start button is preessed!\n");
}

void UdpClient::stopSendDatagram()
{
    disconnect(m_pTimer, &QTimer::timeout, this, &UdpClient::sendDatagram);
    m_pTimer->stop();
    sendSpecialDatagram("Stop button is pressed!\n");
}

void UdpClient::openAbout()
{
    QMessageBox::about(this,
                       tr("About EnteringSettingsApp"),
                       tr("This program allows to enter integer numbers\n"
                          "and send it to another app.\n"
                          "You can start sending data after the pressing \'Start\' button\n"
                          "if the text edit is not empty and stop sending data after\n"
                          "pressing \'Stop\' button."));
}

void UdpClient::createActions()
{
    QMenu* pHelpMenuPoint = menuBar()->addMenu(tr("&Help"));
    QAction* pAboutPoint = pHelpMenuPoint->addAction(tr("&About..."), this, &UdpClient::openAbout);
    pAboutPoint->setShortcut(tr("Ctrl+F"));
    QAction* pExitPoint = pHelpMenuPoint->addAction(tr("&Exit"), this, &QWidget::close);
    pExitPoint->setShortcut(QKeySequence::Close);
}

void UdpClient::addWidgets()
{
    QWidget* pWidgets = new QWidget;
    m_pTextEdit = new QTextEdit;
    QPushButton* pStartButton = new QPushButton("St&art");
    QPushButton* pStopButton = new QPushButton("St&op");
    connect(pStartButton, &QPushButton::clicked, this, &UdpClient::startSendDatagram);
    connect(pStopButton, &QPushButton::clicked, this, &UdpClient::stopSendDatagram);

    readSettings();

    QVBoxLayout* pVBoxLayout = new QVBoxLayout;
    QHBoxLayout* pHBoxLayout = new QHBoxLayout;
    pVBoxLayout->setContentsMargins(5, 5, 5, 5);
    pVBoxLayout->addWidget(m_pTextEdit);
    pHBoxLayout->addWidget(pStartButton);
    pHBoxLayout->addWidget(pStopButton);
    pVBoxLayout->addLayout(pHBoxLayout);
    pWidgets->setLayout(pVBoxLayout);
    setCentralWidget(pWidgets);
}
