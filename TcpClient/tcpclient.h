#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>
#include "opewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public slots:
    void show_connect();
    void recvMsg();
private:
    TcpClient(QWidget *parent = nullptr);

public:
    ~TcpClient();
    void loadConfig(); // 加载配置文件信息
    static TcpClient& getinstance();
    QTcpSocket &getTcpSocket();
    QString loginName();
    QString curPath();

    void setCurPath(QString strCurPath);

private slots:
//    void on_send_pb_clicked();



    void on_login_pb_clicked();

    void on_regist_pd_clicked();

    void on_cancel_pb_clicked();

private:
    Ui::TcpClient *ui;
    QString m_strIP; //IP地址
    quint16 m_strPort; //端口
    QTcpSocket m_tcpsocket;

    QString m_strLoginName;

    QString m_strCurPath;

    QFile m_file;
};
#endif // TCPCLIENT_H
