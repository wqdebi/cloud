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
    loadConfig();
    connect_server();
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

void TcpClient::connect_server()
{
    connect(&m_tcpsocket, SIGNAL(connected()),
            this, SLOT(show_connect()));
    m_tcpsocket.connectToHost(QHostAddress(m_strIP), m_strPort);
}

void TcpClient::show_connect()
{
    qDebug() << "TCP连接成功";
}


void TcpClient::on_send_pb_clicked()
{
    QString str = ui->lineEdit->text();
    if(!str.isEmpty()){
        PDU* pdu = mkPDU(str.size());
        pdu->UiMsgType = 8888;
        memcpy(pdu->caMsg, str.toStdString().c_str(),str.size());
        m_tcpsocket.write((char *)pdu, pdu->uilPDULen);
        free(pdu);
        pdu = NULL;
    }else{
        QMessageBox::warning(this, "消息发送", "发送信息不能为空");
    }

}
