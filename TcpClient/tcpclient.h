#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public slots:
    void show_connect();
public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void loadConfig(); // 加载配置文件信息
    void connect_server();



private:
    Ui::TcpClient *ui;
    QString m_strIP; //IP地址
    quint16 m_strPort; //端口
    QTcpSocket m_tcpsocket;
};
#endif // TCPCLIENT_H
