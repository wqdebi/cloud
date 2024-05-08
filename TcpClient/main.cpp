#include "tcpclient.h"
#include "online.h"
#include "friend.h"
#include "opewidget.h"
//#include "sharefile.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    OpeWidget w;
//    w.show();
//    Friend w;
//    w.show();
//    Online w;
//    w.show();

//    ShareFile w;
//    w.show();

    TcpClient::getinstance().show();

    return a.exec();
}
