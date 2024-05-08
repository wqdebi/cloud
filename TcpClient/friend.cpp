#include "friend.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QDebug>
#include "privatechat.h"
#include <QMessageBox>

Friend::Friend(QWidget *parent) : QWidget(parent)
{
    m_pShowMsgTE = new QTextEdit;
    m_pFriendListWidget = new QListWidget;
    m_pInputMsgLE = new QLineEdit;

    m_pDelFriendPB = new QPushButton("删除好友");
    m_pFlushFriendPB = new QPushButton("刷新好友");
    m_pShowOnlineUsrPB = new QPushButton("显示在线用户");
    m_pSearchUsrPB = new QPushButton("查找好友");
    m_pMsgSendPB = new QPushButton("发送信息");
    m_pPrivateChatPB = new QPushButton("私聊");

    QVBoxLayout *pRightPBVBL = new QVBoxLayout;
    pRightPBVBL->addWidget(m_pDelFriendPB);
    pRightPBVBL->addWidget(m_pFlushFriendPB);
    pRightPBVBL->addWidget(m_pShowOnlineUsrPB);
    pRightPBVBL->addWidget(m_pSearchUsrPB);
    //pRightPBVBL->addWidget(m_pMsgSendPB);
    pRightPBVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVBL);

    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    m_pOnline = new Online;

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();

    setLayout(pMain);

    connect(m_pShowOnlineUsrPB, SIGNAL(clicked(bool)),
            this, SLOT(showOnline()));
    connect(m_pSearchUsrPB, SIGNAL(clicked(bool)),
            this, SLOT(searchUsr()));
    connect(m_pFlushFriendPB, SIGNAL(clicked(bool)),
            this, SLOT(flushFriend()));
    connect(m_pDelFriendPB, SIGNAL(clicked(bool)),
            this, SLOT(delFriend()));
    connect(m_pPrivateChatPB, SIGNAL(clicked(bool)),
            this, SLOT(privateChat()));
    connect(m_pMsgSendPB, SIGNAL(clicked(bool)),
            this, SLOT(groupChat()));
}

void Friend::showAllOnlineUsr(PDU *pdu)
{
    if(NULL == pdu){
        return;
    }
    m_pOnline->showUsr(pdu);
}

void Friend::updateFriendList(PDU *pdu)
{
    if(NULL == pdu){
        return;
    }
    uint uiSize = pdu->uiMsgLen / 32;
    char caName[32] = {'\0'};
    m_pFriendListWidget->clear();
    for(int i = 0; i < (int)uiSize; ++i){
        memcpy(caName, (char *)(pdu->caMsg) + i * 32, 32);
        m_pFriendListWidget->addItem(caName);
    }

}

void Friend::updateGroupMsg(PDU *pdu)
{
    QString strMsg = QString("%1 says: %2").arg(pdu->caData).arg((char *)pdu->caMsg);
    m_pShowMsgTE->append(strMsg);
}

QListWidget *Friend::getFriendList()
{
    return m_pFriendListWidget;
}

void Friend::showOnline()
{
    if(m_pOnline->isHidden()){
        m_pOnline -> show();
        // 发送请求查询数据库获取在线用户
        PDU *pdu = mkPDU(0);
        pdu->UiMsgType = ENUM_MSG_TYPE_ONLINE_USERS_REQUEST;
        TcpClient::getinstance().getTcpSocket().write((char*)pdu, pdu->uilPDULen);
        free(pdu);
        pdu = NULL;

    }else{
        m_pOnline->hide();
    }
}

void Friend::searchUsr()
{
    m_strSearchName = QInputDialog::getText(this, "搜索", "用户名");
    if(!m_strSearchName.isEmpty()){
        PDU *pdu = mkPDU(0);
        pdu->UiMsgType = ENUM_MSG_TYPE_SEARCH_USER_REQUEST;
        memcpy(pdu->caData, m_strSearchName.toStdString().c_str(), m_strSearchName.size());
        TcpClient::getinstance().getTcpSocket().write((char *)pdu, pdu->uilPDULen);
        free(pdu);
        pdu = nullptr;
    }
}

void Friend::flushFriend()
{
    QString strName = TcpClient::getinstance().loginName();
    PDU *pdu = mkPDU(0);
    pdu->UiMsgType = ENUM_MSG_TYPE_FLSUH_FRIEND_REQUEST;
    memcpy(pdu->caData, strName.toStdString().c_str(), strName.size());
    TcpClient::getinstance().getTcpSocket().write((char *)pdu, pdu->uilPDULen);
    free(pdu);
    pdu = nullptr;
}

void Friend::delFriend()
{
    if(NULL != m_pFriendListWidget->currentItem()){
        QString strFriendName = m_pFriendListWidget->currentItem()->text();
        PDU *pdu = mkPDU(0);
        pdu->UiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
        QString strSelfName = TcpClient::getinstance().loginName();
        memcpy(pdu->caData, strSelfName.toStdString().c_str(), strSelfName.size());
        memcpy(pdu->caData + 32, strFriendName.toStdString().c_str(), strSelfName.size());
        TcpClient::getinstance().getTcpSocket().write((char *)pdu, pdu->uilPDULen);
        free(pdu);
        pdu = NULL;
    }

}

void Friend::privateChat()
{
    if(NULL != m_pFriendListWidget->currentItem()){
        QString strChatName = m_pFriendListWidget->currentItem()->text();
        PrivateChat::getinstance().setChatName(strChatName);
        if(PrivateChat::getinstance().isHidden()){
            PrivateChat::getinstance().show();
        }
    }else{
        QMessageBox::warning(this, "私聊", "选择私聊对象");
    }
}

void Friend::groupChat()
{
    QString strMsg = m_pInputMsgLE->text();
    QString mesend = QString("<font color='red'>%1 says: %2</font>")
            .arg(TcpClient::getinstance().loginName()).arg(strMsg);
    m_pShowMsgTE->append(mesend);
    m_pInputMsgLE->clear();
    if(!strMsg.isEmpty()){
        PDU *pdu = mkPDU(strMsg.size() + 1);
        pdu->UiMsgType = ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;
        QString strName = TcpClient::getinstance().loginName();
        strncpy(pdu->caData, strName.toStdString().c_str(), strName.size());
        strncpy((char *)(pdu->caMsg), strMsg.toStdString().c_str(), strMsg.size());
        TcpClient::getinstance().getTcpSocket().write((char *)pdu, pdu->uilPDULen);
    }else{
        QMessageBox::warning(this, "群聊", "信息不能为空");
    }
}
