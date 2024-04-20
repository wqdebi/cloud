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
    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(handle);
    m_tcpSocketList.append(pTcpSocket);
    connect(pTcpSocket, SIGNAL(offline(MyTcpSocket *)),
            this, SLOT(deleteSocket(MyTcpSocket *)));
}

void mytcpser::resend(const char *pername, PDU *pdu)
{
    if(NULL == pername || pdu == nullptr){
        return;
    }
    QString strName = pername;
    for(int i = 0; i < m_tcpSocketList.size(); ++i){
        if(strName == m_tcpSocketList.at(i)->getName()){
            m_tcpSocketList.at(i)->write((char *)pdu, pdu->uilPDULen);
        }
    }
}

void mytcpser::deleteSocket(MyTcpSocket *mysocket)
{
    QList<MyTcpSocket *>::iterator iter = m_tcpSocketList.begin();
    for(; iter != m_tcpSocketList.end(); ++iter){
        if(mysocket == *iter){
             (*iter) -> deleteLater();
            *iter = nullptr;
            m_tcpSocketList.erase(iter);
            break;
        }
    }
//    for(int i = 0; i < m_tcpSocketList.size(); ++i){
//        qDebug() << m_tcpSocketList.at(i)->getName();
//    }
}
