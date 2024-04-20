#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"
#include <QMessageBox>

Online::Online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showUsr(PDU *pdu)
{
    if(NULL == pdu){
        return;
    }
    ui->online_lw->clear();
    uint uiSize = pdu->uiMsgLen / 32;
    char caTmp[32];
    for(uint i = 0; i < uiSize; ++i){
        memcpy(caTmp, (char *)(pdu->caMsg)+ i *32, 32);
        ui->online_lw->addItem(caTmp);
    }
}

void Online::on_addFriend_pb_clicked()
{
    QListWidgetItem *pItem =  ui->online_lw->currentItem();
    if(pItem != nullptr){
        QString strPerUsrName = pItem->text();
        QString strLoginName = TcpClient::getinstance().loginName();
        if(strPerUsrName == strLoginName){
            QMessageBox::warning(this, "添加好友", "不能添加自己为好友");
            return;
        }
        PDU *pdu = mkPDU(0);
        pdu->UiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
        memcpy(pdu->caData, strPerUsrName.toStdString().c_str(), strPerUsrName.size());
        memcpy(pdu->caData + 32, strLoginName.toStdString().c_str(), strLoginName.size());
        TcpClient::getinstance().getTcpSocket().write((char *)pdu, pdu->uilPDULen);
        free(pdu);
        pdu = nullptr;
    }
}
