#ifndef MYTCPSER_H
#define MYTCPSER_H
#include <QTcpServer>
#include <QList>
#include "mytcpsocket.h"


class mytcpser : public QTcpServer
{
    Q_OBJECT
private:
    mytcpser();
public:
    static mytcpser& getInstance();

    void incomingConnection(qintptr handle) override;

private:
    QList<MyTcpSocket *> m_tcpSocketList;
};

#endif // MYTCPSER_H
