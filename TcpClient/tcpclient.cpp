#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();
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

