#include "privatechat.h"
#include "ui_privatechat.h"
#include "protocal.h"
#include "tcpclient.h"
#include <QMessageBox>
#include "string.h"

PrivateChat::PrivateChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrivateChat)
{
    ui->setupUi(this);
}

PrivateChat::~PrivateChat()
{
    delete ui;
}

void PrivateChat::setChatName(QString Name)
{
    m_strChatName = Name;
    m_strLoginName = TcpClient::getinstance().loginName();
}

PrivateChat &PrivateChat::getinstance()
{
    static PrivateChat instance;
    return instance;
}

void PrivateChat::updataMsg(const PDU *pdu)
{
    if(NULL == pdu){
        return;
    }
    char caSendName[32] = {'\0'};
    memcpy(caSendName, pdu->caData, 32);
    QString strMsg = QString("%1 says: %2").arg(caSendName).arg((char *)pdu->caMsg);
    ui->showMsg_te->append(strMsg);
}

void PrivateChat::on_sendMsg_pb_clicked()
{
    QString strMsg = ui->inputMsg_le->text();
    ui->inputMsg_le->clear();
    if(!strMsg.isEmpty()){
        QString mesend = QString("<font color='red'>%1 says: %2</font>")
                .arg(TcpClient::getinstance().loginName()).arg(strMsg);
        ui->showMsg_te->append(mesend);
        PDU *pdu = mkPDU(strMsg.size() + 1);
        pdu->UiMsgType = ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;
        memcpy(pdu->caData, m_strLoginName.toStdString().c_str(), m_strLoginName.size());
        memcpy(pdu->caData + 32, m_strChatName.toStdString().c_str(), m_strChatName.size());
        strcpy((char *)pdu->caMsg, strMsg.toStdString().c_str());
        TcpClient::getinstance().getTcpSocket().write((char *)pdu, pdu->uilPDULen);
        free(pdu);
        pdu = NULL;
    }else{
        QMessageBox::warning(this, "私聊", "聊天信息不能为空");
    }
}
