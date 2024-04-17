#include "tcpserver.h"
#include "ui_tcpserver.h"
#include <QByteArray>
#include <QDebug> // 调试
#include <QMessageBox> // 消息提示框
#include <QHostAddress>
#include <QFile>
#include "mytcpser.h"

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();
    mytcpser::getInstance().listen(QHostAddress(m_strIP), m_strPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config"); // 文件对象，读取资源文件 ':' + "前缀" + "文件名"
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

