#include "mytcpser.h"
#include <QDebug>

mytcpser::mytcpser()
{

}

mytcpser &mytcpser::getInstance()
{
    static mytcpser instance;
    return instance;
}

void mytcpser::incomingConnection(qintptr handle)
{
    qDebug() << "new client connected";
}
