#include "tcpclient.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("TestResume");
    QApplication a(argc, argv);
    TcpClient w("localhost", 1237);
    w.show();
    return a.exec();
}
