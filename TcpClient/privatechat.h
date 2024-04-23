#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>
#include "protocal.h"

namespace Ui {
class PrivateChat;
}

class PrivateChat : public QWidget
{
    Q_OBJECT

private:
    explicit PrivateChat(QWidget *parent = nullptr);

public:
    ~PrivateChat();

    void setChatName(QString Name);
    static PrivateChat& getinstance();
    void updataMsg(const PDU* pdu);
private slots:
    void on_sendMsg_pb_clicked();

private:
    Ui::PrivateChat *ui;
    QString m_strChatName;
    QString m_strLoginName;
};

#endif // PRIVATECHAT_H
