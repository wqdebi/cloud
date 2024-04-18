#include "mytcpsocket.h"
#include <QDebug>
#include "protocal.h"


MyTcpSocket::MyTcpSocket()
{
    connect(this, SIGNAL(readyRead()),
                         this, SLOT(recvMsg()));
}

void MyTcpSocket::recvMsg()
{
    qDebug() << this->bytesAvailable();
    uint uiPDUlen = 0;
    this->read((char *)&uiPDUlen, sizeof(uint));
    uint uiMsgLen = uiPDUlen - sizeof(PDU);
    PDU* pdu = mkPDU(uiMsgLen);
    this->read((char *)pdu + sizeof(uint), uiPDUlen - sizeof(uint));
    qDebug() << pdu->UiMsgType << (char *)pdu->caMsg;
}
