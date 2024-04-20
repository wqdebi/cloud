#include "friend.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QDebug>

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
    for(int i = 0; i < uiSize; ++i){
        memcpy(caName, (char *)(pdu->caMsg) + i * 32, 32);
        m_pFriendListWidget->addItem(caName);
    }

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
