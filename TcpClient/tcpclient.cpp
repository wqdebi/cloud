#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug> // 调试
#include <QMessageBox> // 消息提示框
#include <QHostAddress>
#include "protocal.h"

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    resize(500, 250);
    loadConfig();
    connect(&m_tcpsocket, SIGNAL(connected()),
            this, SLOT(show_connect()));
    connect(&m_tcpsocket, SIGNAL(readyRead()),
            this, SLOT(recvMsg()));
    m_tcpsocket.connectToHost(QHostAddress(m_strIP), m_strPort);
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{
    QFile file(":/client.config"); // 文件对象，读取资源文件 ':' + "前缀" + "文件名"
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strDate = baData.toStdString().c_str();
        file.close();
        strDate.replace("\r\n", " ");
        QStringList List = strDate.split(" ");
        m_strIP = List.at(0);
        m_strPort = List.at(1).toUShort();
        qDebug() << "IP: " << m_strIP << " Port: " << m_strPort;
    }else{
        QMessageBox::critical(this, "open config", "open config failed");

    }
}

TcpClient &TcpClient::getinstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpsocket;
}


void TcpClient::show_connect()
{
    qDebug() << "TCP连接成功";
}

void TcpClient::recvMsg()
{
    qDebug() << m_tcpsocket.bytesAvailable();
    uint uiPDUlen = 0;
    m_tcpsocket.read((char *)&uiPDUlen, sizeof(uint));
    uint uiMsgLen = uiPDUlen - sizeof(PDU);
    PDU* pdu = mkPDU(uiMsgLen);
    m_tcpsocket.read((char *)pdu + sizeof(uint), uiPDUlen - sizeof(uint));
//    qDebug() << pdu->UiMsgType << (char *)pdu->caMsg;
//    qDebug() << "ok" <<pdu->UiMsgType;
    switch(pdu->UiMsgType){
    case ENUM_MSG_TYPE_REGIST_RESPOND:{
        if(0 == strcmp(pdu->caData, REGIST_OK)){
            QMessageBox::information(this, "注册", REGIST_OK);
        }else if(0 == strcmp(pdu->caData, REGIST_FAILED)){
            QMessageBox::warning(this, "注册", REGIST_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND:{
        if(0 == strcmp(pdu->caData, LOGIN_OK)){
            QMessageBox::information(this, "登录", LOGIN_OK);
            OpeWidget::getInstance().show();
            this->hide();

        }else if(0 == strcmp(pdu->caData, LOGIN_FAILED)){
            QMessageBox::warning(this, "登录", LOGIN_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_ONLINE_USERS_RESPOND:{
        OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);
        break;
    }
    default:
            break;
    }
    free(pdu);
    pdu = NULL;
}


//void TcpClient::on_send_pb_clicked()
//{
//    QString str = ui->lineEdit->text();
//    if(!str.isEmpty()){
//        PDU* pdu = mkPDU(str.size());
//        pdu->UiMsgType = 8888;
//        memcpy(pdu->caMsg, str.toStdString().c_str(),str.size());
//        m_tcpsocket.write((char *)pdu, pdu->uilPDULen);
//        free(pdu);
//        pdu = NULL;
//    }else{
//        QMessageBox::warning(this, "消息发送", "发送信息不能为空");
//    }

//}




void TcpClient::on_login_pb_clicked()
{
    QString strName = ui -> name_le -> text(); // 获取用户名和密码
    QString strPwd = ui -> pwd_le -> text();
    // 合理性判断
    if(!strName.isEmpty() && !strPwd.isEmpty())
    {
        // 注册信息用户名和密码将通过caData[64]传输
        PDU *pdu = mkPDU(0); // 实际消息体积为0
        pdu->UiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST; // 设置为注册请求消息类型
        // 拷贝用户名和密码信息到caData
        memcpy(pdu->caData, strName.toStdString().c_str(), 32); // 由于数据库设定的32位，所以最多只拷贝前32位
        memcpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
        // qDebug() << pdu -> uiMsgType << " " << pdu -> caData << " " << pdu -> caData + 32;
        m_tcpsocket.write((char*)pdu, pdu->uilPDULen); // 发送消息

        // 释放空间
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this, "登录", "登录失败：用户名或密码为空！");
    }
}

void TcpClient::on_regist_pd_clicked()
{
    QString strName = ui -> name_le -> text(); // 获取用户名和密码
    QString strPwd = ui -> pwd_le -> text();
    // 合理性判断
    if(!strName.isEmpty() && !strPwd.isEmpty())
    {
        // 注册信息用户名和密码将通过caData[64]传输
        PDU *pdu = mkPDU(0); // 实际消息体积为0
        pdu->UiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST; // 设置为注册请求消息类型
        // 拷贝用户名和密码信息到caData
        memcpy(pdu->caData, strName.toStdString().c_str(), 32); // 由于数据库设定的32位，所以最多只拷贝前32位
        memcpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
        // qDebug() << pdu -> uiMsgType << " " << pdu -> caData << " " << pdu -> caData + 32;
        m_tcpsocket.write((char*)pdu, pdu->uilPDULen); // 发送消息

        // 释放空间
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this, "注册", "注册失败：用户名或密码为空！");
    }
}

void TcpClient::on_cancel_pb_clicked()
{

}
