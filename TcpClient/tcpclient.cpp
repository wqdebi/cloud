#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug> // 调试
#include <QMessageBox> // 消息提示框
#include <QHostAddress>
#include "protocal.h"
#include <QCoreApplication>
#include "privatechat.h"

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

QString TcpClient::loginName()
{
    return m_strLoginName;
}

QString TcpClient::curPath()
{
    return m_strCurPath;
}

void TcpClient::setCurPath(QString strCurPath)
{
    m_strCurPath = strCurPath;
}


void TcpClient::show_connect()
{
    qDebug() << "TCP连接成功";
}

void TcpClient::recvMsg()
{
    if(OpeWidget::getInstance().getBook()->getDownloadStatus()){
        QByteArray buffer =  m_tcpsocket.readAll();

        m_file.write(buffer);
        Book *pBook = OpeWidget::getInstance().getBook();
        pBook->m_iRecved += buffer.size();
        if(pBook->m_iTotal == pBook->m_iRecved){
            m_file.close();
            pBook->m_iTotal = 0;
            pBook->m_iRecved = 0;
            pBook->setDownloadStatus(false);
            m_file.setFileName("");
            QMessageBox::information(this, "下载文件", "下载文件完成");
        }else if(pBook->m_iTotal < pBook->m_iRecved){
            m_file.close();
            pBook->m_iTotal = 0;
            pBook->m_iRecved = 0;
            pBook->setDownloadStatus(false);
            m_file.setFileName("");
            QMessageBox::critical(this, "下载文件", "下载文件失败");
        }
        //QMessageBox::information(this, "下载文件", "okdfa ");
    }
    //qDebug() << m_tcpsocket.bytesAvailable();
    else{
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
            m_strCurPath = QString("./%1").arg(m_strLoginName);
            QMessageBox::information(this, "注册", REGIST_OK);
        }else if(0 == strcmp(pdu->caData, REGIST_FAILED)){
            QMessageBox::warning(this, "注册", REGIST_FAILED);
        }

        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND:{
        if(0 == strcmp(pdu->caData, LOGIN_OK)){
            m_strCurPath = QString("./%1").arg(m_strLoginName);
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
    case ENUM_MSG_TYPE_SEARCH_USER_RESPOND:{
        if(0 == strcmp(SEARCH_USER_EMPTY, pdu->caData)){
            QString str = QString("%1: not exist").arg(OpeWidget::getInstance().getFriend()->m_strSearchName);
            QMessageBox::information(this, "搜索", str);
        }else if(0 == strcmp(SEARCH_USER_OK, pdu->caData)){
            QString str = QString("%1: online").arg(OpeWidget::getInstance().getFriend()->m_strSearchName);
            QMessageBox::information(this, "搜索", str);
        }else if(0 == strcmp(SEARCH_USER_OFFLINE, pdu->caData)){
            QString str = QString("%1: offline").arg(OpeWidget::getInstance().getFriend()->m_strSearchName);
            QMessageBox::information(this, "搜索", str);
        }

        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
        char sourceName[32]; // 获取发送方用户名
        strncpy(sourceName, pdu -> caData + 32, 32);
        int ret = QMessageBox::information(this, "好友申请", QString("%1 想添加您为好友，是否同意？").arg(sourceName),
                                 QMessageBox::Yes, QMessageBox::No); // 后面两个参数是为QMessage默认支持两个按钮来设置枚举值
        PDU* resPdu = mkPDU(0);

        strncpy(resPdu -> caData, pdu -> caData, 32); // 被加好友者用户名
        strncpy(resPdu -> caData + 32, pdu -> caData + 32, 32); // 加好友者用户名
        // qDebug() << "同意加好友吗？" << resPdu -> caData << " " << resPdu -> caData + 32;
        if(ret == QMessageBox::Yes) // 同意加好友
        {
            resPdu->UiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE;
        }
        else
        {
            resPdu->UiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REJECT;
        }
        m_tcpsocket.write((char*)resPdu, resPdu->uilPDULen); // 发送给服务器消息，由服务器写入数据库并转发给用户
        free(resPdu);
        resPdu=nullptr;

        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:{
        QMessageBox::information(this, "添加好友", pdu->caData);

        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE: // 对方同意加好友
    {
        QMessageBox::information(this, "添加好友", QString("%1 已同意您的好友申请!").arg(pdu -> caData));

        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REJECT: // 对方拒绝加好友
    {
        QMessageBox::information(this, "添加好友", QString("%1 已拒绝您的好友申请！").arg(pdu -> caData));

        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:{
        OpeWidget::getInstance().getFriend()->updateFriendList(pdu);

        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:{
        char caname[32] = {'\0'};
        memcpy(caname, pdu->caData, 32);
        QMessageBox::information(this, "删除好友", QString("%1 删除了你").arg(caname));

        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:{
        QMessageBox::information(this, "删除好友", "删除好友成功");

        break;
    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:{
        if(PrivateChat::getinstance().isHidden()){
            PrivateChat::getinstance().show();
        }
        char perName[32] = {'\0'};
        memcpy(perName, pdu->caData, 32);
        QString strSendName = perName;
        PrivateChat::getinstance().setChatName(strSendName);
        PrivateChat::getinstance().updataMsg(pdu);

        break;
    }
    case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:{
        OpeWidget::getInstance().getFriend()->updateGroupMsg(pdu);

        break;
    }
    case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:{
        QMessageBox::information(this, "创建文件夹", pdu->caData);

        break;
    }
    case ENUM_MSG_TYPE_FLUSH_DIR_RESPOND:{
        OpeWidget::getInstance().getBook()->updateFileList(pdu);
        QString strEnterDir = OpeWidget::getInstance().getBook()->enterDir();
        if(!strEnterDir.isEmpty()){
            m_strCurPath = m_strCurPath + "/" + strEnterDir;
            OpeWidget::getInstance().getBook()->clearEnterDir();
            qDebug() << "enter dir" << m_strCurPath;
        }else{

        }

        break;
    }
    case ENUM_MSG_TYPE_DEL_DIR_RESPOND:{
        QMessageBox::information(this, "删除文件夹", pdu->caData);

        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_RESPOND:{
//        qDebug() << 114514;
        QMessageBox::information(this, "重命名文件", pdu->caData);

        break;
    }
    case ENUM_MSG_TYPE_ENTRY_DIR_RESPOND:{
        OpeWidget::getInstance().getBook()->clearEnterDir();
        QMessageBox::information(this, "进入文件夹", pdu->caData);

        break;
    }
    case ENUM_MSG_TYPE_DELETE_FILE_RESPOND:{
        QMessageBox::information(this, "删除文件", pdu->caData);

        break;
    }

    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:{
        QMessageBox::information(this, "上传文件", pdu->caData);

        break;
    }
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND:{
        qDebug() << pdu->caData;
        char caFileName[32] = {'\0'};
        sscanf(pdu->caData, "%s %lld", caFileName,
               &(OpeWidget::getInstance().getBook()->m_iTotal));
        if(strlen(caFileName) > 0 && OpeWidget::getInstance().getBook()->m_iTotal > 0){
            OpeWidget::getInstance().getBook()->setDownloadStatus(true);
            m_file.setFileName(OpeWidget::getInstance().getBook()->getSaveFilePath());
            if(!m_file.open(QIODevice::WriteOnly)){
                QMessageBox::warning(this, "下载文件", "下载文件失败");
            }
        }
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:{
        QMessageBox::information(this, "共享文件", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_NOTE:{
        char *pPath = new char[pdu->uiMsgLen];
        memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
        char *pos = strrchr(pPath, '/');
        if(NULL != pos){
            ++pos;
            QString strNote = QString("%1 share file -> %2\n do you want to accept").arg(pdu->caData).arg(pos);
            int ret = QMessageBox::question(this, "共享文件", strNote);
            if(QMessageBox::Yes == ret){
                PDU *respdu = mkPDU(pdu->uiMsgLen);
                respdu->UiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND;
                memcpy(respdu->caMsg, pdu->caMsg, pdu->uiMsgLen);
                QString strName = TcpClient::getinstance().loginName();
                strcpy(respdu->caData, strName.toStdString().c_str());
                m_tcpsocket.write((char *)respdu, respdu->uilPDULen);
            }
        }
        delete [] pPath;
        break;
    }
    case ENUM_MSG_TYPE_MOVE_FILE_RESPOND:{
        QMessageBox::information(this, "移动文件", pdu->caData);
        break;
    }
    default:
        break;
    }
    free(pdu);
    pdu = NULL;
    }
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
        m_strLoginName = strName;
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
