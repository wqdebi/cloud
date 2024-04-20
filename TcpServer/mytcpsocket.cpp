#include "mytcpsocket.h"
#include <QDebug>
#include "protocal.h"
#include <string.h>


MyTcpSocket::MyTcpSocket()
{
    connect(this, SIGNAL(readyRead()),
                         this, SLOT(recvMsg()));
    connect(this, SIGNAL(disconnected()),
            this, SLOT(clientOffline()));
}

QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::recvMsg()
{
    qDebug() << this->bytesAvailable();
    uint uiPDUlen = 0;
    this->read((char *)&uiPDUlen, sizeof(uint));
    uint uiMsgLen = uiPDUlen - sizeof(PDU);
    PDU* pdu = mkPDU(uiMsgLen);
    this->read((char *)pdu + sizeof(uint), uiPDUlen - sizeof(uint));
//    qDebug() << pdu->UiMsgType << (char *)pdu->caMsg;
    switch(pdu->UiMsgType){
    case ENUM_MSG_TYPE_REGIST_REQUEST:{
        char caName[32] = {'\0'};
        char caPwd[32] = {'\0'};
        strncpy(caName, pdu->caData, 32);
        strncpy(caPwd, pdu->caData+32, 32);
//        qDebug() << "caName:" << caName << "caPwd:" << caPwd << "UiMsgType" << pdu->UiMsgType;
        bool ret = OPeDb::getInstance().handleRegist(caName, caPwd);
        PDU* respdu = mkPDU(0);
        respdu->UiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
        if(ret){
            strcpy(respdu->caData, REGIST_OK);
        }else{
            strcpy(respdu->caData, REGIST_FAILED);
        }
        write((char*)respdu, respdu->uilPDULen); // 发送消息

        // 释放空间
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_REQUEST:{
        char caName[32] = {'\0'};
        char caPwd[32] = {'\0'};
        strncpy(caName, pdu->caData, 32);
        strncpy(caPwd, pdu->caData+32, 32);
        bool ret = OPeDb::getInstance().handleLogin(caName, caPwd);
        PDU* respdu = mkPDU(0);
        respdu->UiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
        if(ret){
            strcpy(respdu->caData, LOGIN_OK);
            m_strName = caName;
        }else{
            strcpy(respdu->caData, LOGIN_FAILED);
        }
        write((char*)respdu, respdu->uilPDULen); // 发送消息

        // 释放空间
        free(respdu);
        respdu = NULL;
        break;
        break;
    }
    case ENUM_MSG_TYPE_ONLINE_USERS_REQUEST:{
        QStringList ret = OPeDb::getInstance().handleAllOnline();
        uint uiMsgLen = ret.size() * 32;
        PDU *respdu = mkPDU(uiMsgLen);
        respdu->UiMsgType = ENUM_MSG_TYPE_ONLINE_USERS_RESPOND;
        qDebug() << ret.size();
        for(int i = 0; i < ret.size(); ++i){
            memcpy((char *)(respdu->caMsg) + i * 32,
                   ret.at(i).toStdString().c_str(),
                   ret.at(i).size());
            qDebug() << ret.at(i);
        }
        write((char *)respdu, respdu->uilPDULen);
        free(respdu);
        respdu = nullptr;
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USER_REQUEST:{
        int ret = OPeDb::getInstance().handleSearchUsr(pdu->caData);
        PDU *respdu = mkPDU(0);
        respdu->UiMsgType = ENUM_MSG_TYPE_SEARCH_USER_RESPOND;
        if(ret == -1){
            strcpy(respdu->caData, SEARCH_USER_EMPTY);
        }else if(ret == 0){
            strcpy(respdu->caData, SEARCH_USER_OFFLINE);
        }else if(ret == 1){
            strcpy(respdu->caData, SEARCH_USER_OK);
        }
        write((char *)respdu, respdu->uilPDULen);
        free(respdu);
        respdu = nullptr;
        break;
    }
    default:
        break;
    }
    free(pdu);
    pdu = NULL;
}

void MyTcpSocket::clientOffline()
{
    OPeDb::getInstance().handleOffline(m_strName.toStdString().c_str());
    emit offline(this);
}
