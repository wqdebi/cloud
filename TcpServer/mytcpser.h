#ifndef MYTCPSER_H
#define MYTCPSER_H
#include <QTcpServer>
#include <QList>
#include "mytcpsocket.h"
#include "protocal.h"


class mytcpser : public QTcpServer
{
    Q_OBJECT
private:
    mytcpser();
public:
    static mytcpser& getInstance();

    void incomingConnection(qintptr handle) override;

    void resend(const char *pername, PDU *pdu);

public slots:
    void deleteSocket(MyTcpSocket *mysocket);

private:
    QList<MyTcpSocket *> m_tcpSocketList;
};

#endif // MYTCPSER_H
