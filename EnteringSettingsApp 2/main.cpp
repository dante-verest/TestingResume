#include "udpclient.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("TestResume");
    QApplication a(argc, argv);
    UdpClient w;
    w.show();
    return a.exec();
}
