#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>
#include "mytcpsocket.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TcpServer; }
QT_END_NAMESPACE

class TcpServer : public QWidget
{
    Q_OBJECT

public:
    TcpServer(QWidget *parent = nullptr);
    ~TcpServer();
    void loadConfig(); // 加载配置文件信息



private:
    Ui::TcpServer *ui;

    QString m_strIP; //IP地址
    quint16 m_strPort; //端口
};
#endif // TCPSERVER_H
