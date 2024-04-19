#include "tcpclient.h"
#include "online.h"
#include "friend.h"
#include "opewidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpeWidget w;
    w.show();
//    Friend w;
//    w.show();
//    Online w;
//    w.show();

//    TcpClient w;
//    w.show();
    return a.exec();
}
