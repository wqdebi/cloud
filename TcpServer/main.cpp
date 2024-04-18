#include "tcpserver.h"

#include <QApplication>

#include "opedb.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    OPeDb::getInstance().init();

    TcpServer w;
    w.show();
    return a.exec();
}
