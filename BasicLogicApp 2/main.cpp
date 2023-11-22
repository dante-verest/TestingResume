#include "server.h"

#include <iostream>
#include <QCoreApplication>
#include <QTime>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    std::cout << "The BasicLogicApp started at " <<
                 QTime::currentTime().toString("hh:mm:ss.zzz").toStdString() << "." << std::endl;
    std::cout << "Ctrl+C for closing the terminal correctly." << std::endl;
    Server server(1237);
    return a.exec();
}
