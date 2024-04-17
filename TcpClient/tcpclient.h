#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void loadConfig(); // 加载配置文件信息

private:
    Ui::TcpClient *ui;
    QString m_strIP; //IP地址
    quint16 m_strPort; //
};
#endif // TCPCLIENT_H
