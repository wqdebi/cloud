#include "mytcpsocket.h"
#include <QDebug>
#include "protocal.h"
#include <string.h>
#include "mytcpser.h"
#include <QDir>
#include <QFileInfoList>
#include <QDateTime>



MyTcpSocket::MyTcpSocket()
{

    m_bUpload = false;
    m_pTimer = new QTimer;

    connect(this, SIGNAL(readyRead()),
                         this, SLOT(recvMsg()));
    connect(this, SIGNAL(disconnected()),
            this, SLOT(clientOffline()));
    connect(m_pTimer, SIGNAL(timeout()),
            this, SLOT(sendFileToClient()));
}

QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::copyDir(QString strSrcDir, QString strDestDir)
{
    QDir dir;
    dir.mkdir(strDestDir);
    dir.setPath(strSrcDir);

    QString srcTmp;
    QString destTmp;

    QFileInfoList fileInfoList = dir.entryInfoList();
    for(int i = 0; i < fileInfoList.size(); ++i){
        if(fileInfoList[i].isFile()){
            fileInfoList[i].fileName();
            srcTmp = strSrcDir + '/' + fileInfoList[i].fileName();
            destTmp = strDestDir + '/' + fileInfoList[i].fileName();
            QFile::copy(srcTmp, destTmp);
        }else if(fileInfoList[i].isDir()){
            if(QString(".") == fileInfoList[i].fileName() ||
                    QString("..") == fileInfoList[i].fileName()){
                continue;
            }
            srcTmp = strSrcDir + '/' + fileInfoList[i].fileName();
            destTmp = strDestDir + '/' + fileInfoList[i].fileName();
            copyDir(srcTmp, destTmp);
        }
    }
}

void MyTcpSocket::recvMsg()
{
    if(!m_bUpload){
        qDebug() << this->bytesAvailable();
        uint uiPDUlen = 0;
        this->read((char *)&uiPDUlen, sizeof(uint));
        uint uiMsgLen = uiPDUlen - sizeof(PDU);
        PDU* pdu = mkPDU(uiMsgLen);
        this->read((char *)pdu + sizeof(uint), uiPDUlen - sizeof(uint));
    //    qDebug() << pdu->UiMsgType << (char *)pdu->caMsg;
        switch(pdu->UiMsgType){
        case ENUM_MSG_TYPE_REGIST_REQUEST:{
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName, pdu->caData, 32);
            strncpy(caPwd, pdu->caData+32, 32);
    //        qDebug() << "caName:" << caName << "caPwd:" << caPwd << "UiMsgType" << pdu->UiMsgType;
            bool ret = OPeDb::getInstance().handleRegist(caName, caPwd);
            PDU* respdu = mkPDU(0);
            respdu->UiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
            if(ret){
                strcpy(respdu->caData, REGIST_OK);
                QDir dir;
                qDebug() << "create dir: " << dir.mkdir(QString("./%1").arg(caName));
            }else{
                strcpy(respdu->caData, REGIST_FAILED);
            }
            write((char*)respdu, respdu->uilPDULen); // 发送消息

            // 释放空间
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_LOGIN_REQUEST:{
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName, pdu->caData, 32);
            strncpy(caPwd, pdu->caData+32, 32);
            bool ret = OPeDb::getInstance().handleLogin(caName, caPwd);
            PDU* respdu = mkPDU(0);
            respdu->UiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
            if(ret){
                strcpy(respdu->caData, LOGIN_OK);
                m_strName = caName;
            }else{
                strcpy(respdu->caData, LOGIN_FAILED);
            }
            write((char*)respdu, respdu->uilPDULen); // 发送消息

            // 释放空间
            free(respdu);
            respdu = NULL;
            break;
            break;
        }
        case ENUM_MSG_TYPE_ONLINE_USERS_REQUEST:{
            QStringList ret = OPeDb::getInstance().handleAllOnline();
            uint uiMsgLen = ret.size() * 32;
            PDU *respdu = mkPDU(uiMsgLen);
            respdu->UiMsgType = ENUM_MSG_TYPE_ONLINE_USERS_RESPOND;
            qDebug() << ret.size();
            for(int i = 0; i < ret.size(); ++i){
                memcpy((char *)(respdu->caMsg) + i * 32,
                       ret.at(i).toStdString().c_str(),
                       ret.at(i).size());
                qDebug() << ret.at(i);
            }
            write((char *)respdu, respdu->uilPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case ENUM_MSG_TYPE_SEARCH_USER_REQUEST:{
            int ret = OPeDb::getInstance().handleSearchUsr(pdu->caData);
            PDU *respdu = mkPDU(0);
            respdu->UiMsgType = ENUM_MSG_TYPE_SEARCH_USER_RESPOND;
            if(ret == -1){
                strcpy(respdu->caData, SEARCH_USER_EMPTY);
            }else if(ret == 0){
                strcpy(respdu->caData, SEARCH_USER_OFFLINE);
            }else if(ret == 1){
                strcpy(respdu->caData, SEARCH_USER_OK);
            }
            write((char *)respdu, respdu->uilPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
            char caPerName[32] = {'\0'};
            char caName[32] = {'\0'};
            strncpy(caPerName, pdu->caData, 32);
            strncpy(caName, pdu->caData+32, 32);
            int ret = OPeDb::getInstance().handleAddFriend(caPerName, caName);
            PDU *respdu = nullptr;
            if(-1 == ret){
                respdu = mkPDU(0);
                respdu->UiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, UNKNOWN_ERROR);
                write((char *)respdu, respdu->uilPDULen);
                free(respdu);
                respdu = nullptr;
            }else if(0 == ret){
                respdu = mkPDU(0);
                respdu->UiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, ADD_FRIEND_EXIST);
                write((char *)respdu, respdu->uilPDULen);
                free(respdu);
                respdu = nullptr;
            }else if(1 == ret){
                mytcpser::getInstance().resend(caPerName, pdu);
            }else if(2 == ret){
                respdu = mkPDU(0);
                respdu->UiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, ADD_FRIEND_OFFLINE);
                write((char *)respdu, respdu->uilPDULen);
                free(respdu);
                respdu = nullptr;
            }else if(3 == ret){
                respdu = mkPDU(0);
                respdu->UiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, ADD_FRIEND_EMPTY);
                write((char *)respdu, respdu->uilPDULen);
                free(respdu);
                respdu = nullptr;
            }
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_AGREE: // 同意加好友
        {
            char addedName[32] = {'\0'};
            char sourceName[32] = {'\0'};
            // 拷贝读取的信息
            strncpy(addedName, pdu -> caData, 32);
            strncpy(sourceName, pdu -> caData + 32, 32);

            // 将新的好友关系信息写入数据库
            OPeDb::getInstance().handleAddFriendAgree(addedName, sourceName);

            // 服务器需要转发给发送好友请求方其被同意的消息
            mytcpser::getInstance().resend(sourceName, pdu);
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REJECT: // 拒绝加好友
        {
            char sourceName[32] = {'\0'};
                // 拷贝读取的信息
            strncpy(sourceName, pdu -> caData + 32, 32);
                // 服务器需要转发给发送好友请求方其被拒绝的消息
            mytcpser::getInstance().resend(sourceName, pdu);
            break;
        }
        case ENUM_MSG_TYPE_FLSUH_FRIEND_REQUEST:{
            char sourceName[32] = {'\0'};
            strncpy(sourceName, pdu -> caData, 32);
            QStringList ret = OPeDb::getInstance().handleFlushFriend(sourceName);
            uint uiMsgLen = ret.size() * 32;
            PDU *respdu = mkPDU(uiMsgLen);
            respdu->UiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
            for(int i = 0; i < ret.size(); ++i){
                memcpy((char *)(respdu->caMsg) + i * 32, ret.at(i).toStdString().c_str(),
                       ret.at(i).size());
            }
            write((char *)respdu, respdu->uilPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:{
            char caSelfName[32] = {'\0'};
            char caFriendName[32] = {'\0'};
            // 拷贝读取的信息
            strncpy(caSelfName, pdu -> caData, 32);
            strncpy(caFriendName, pdu -> caData + 32, 32);
            OPeDb::getInstance().handleDelFriend(caSelfName, caFriendName);

            PDU *respdu = mkPDU(0);
            respdu->UiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
            strcpy(respdu->caData, DEL_FRIEND_OK);
            write((char *)respdu, respdu->uilPDULen);
            free(respdu);
            respdu = nullptr;

            mytcpser::getInstance().resend(caFriendName, pdu);

            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:{
            char caPerName[32] = {'\0'};
            memcpy(caPerName, pdu->caData + 32, 32);
            mytcpser::getInstance().resend(caPerName, pdu);
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:{
            char caName[32] = {'\0'};
            strncpy(caName, pdu->caData, 32);
            QStringList onlineFriend = OPeDb::getInstance().handleFlushFriend(caName);
            for(int i = 0; i < onlineFriend.size(); ++i){
                mytcpser::getInstance().resend(
                            onlineFriend.at(i).toStdString().c_str(), pdu);
            }
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:{
            QDir dir;
            QString strCurPath = QString("%1").arg((char *)pdu->caMsg);
            bool ret = dir.exists(strCurPath);
            PDU *respdu;
            if(ret){
                char caNewDir[32] = {'\0'};
                memcpy(caNewDir, pdu->caData + 32, 32);
                QString strNewPath = strCurPath + "/" + caNewDir;
                qDebug() << strNewPath;
                ret = dir.exists(strNewPath);
                if(ret){
                    respdu = mkPDU(0);
                    respdu->UiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respdu->caData, FILE_NAME_EXIST);
                }else{
                    dir.mkdir(strNewPath);
                    respdu = mkPDU(0);
                    respdu->UiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respdu->caData, CREATE_DIR_OK);
                }
            }else{
                respdu = mkPDU(0);
                respdu->UiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                qDebug() << strCurPath;
                strcpy(respdu->caData, DIR_NOT_EXIST);
            }
            write((char *)respdu, respdu->uilPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_DIR_REQUEST:{
            char caCurDir[pdu->uiMsgLen];
            memcpy(caCurDir, (char*)pdu -> caMsg, pdu -> uiMsgLen);
            qDebug() << "刷新文件夹：" << caCurDir;
            QDir dir;
            PDU* respdu = NULL;

            if(!dir.exists(caCurDir)) // 请求文件夹不存在
            {
                respdu = mkPDU(0);
                strncpy(respdu -> caData, PATH_NOT_EXIST, 32);
            }
            else // 存在
            {
                dir.setPath(caCurDir); // 设置为当前目录
                QFileInfoList fileInfoList = dir.entryInfoList(); // 获取当前目录下所有文件
                int iFileNum = fileInfoList.size();

                respdu = mkPDU(sizeof(FileInfo) * iFileNum);
                FileInfo *pFileInfo = NULL; // 创建一个文件信息结构体指针，方便之后遍历PDU空间来赋值
                strncpy(respdu -> caData, FLUSH_DIR_OK, 32);

                for(int i = 0; i < iFileNum; ++ i)
                {
                    pFileInfo = (FileInfo*)(respdu -> caMsg) + i; // 通过指针指向，直接修改PDU空间值，每次偏移FileInfo大小
                    memcpy(pFileInfo -> caName, fileInfoList[i].fileName().toStdString().c_str(), fileInfoList[i].fileName().size());
                    pFileInfo -> bIsDir = fileInfoList[i].isDir();
                    pFileInfo -> uiSize = fileInfoList[i].size();
                    QDateTime dtLastTime = fileInfoList[i].lastModified(); // 获取文件最后修改时间
                    QString strLastTime = dtLastTime.toString("yyyy/MM/dd hh:mm");
                    memcpy(pFileInfo -> caTime, strLastTime.toStdString().c_str(), strLastTime.size());
                    qDebug() << "文件信息：" << pFileInfo -> caName << " " << pFileInfo -> bIsDir << " " << pFileInfo -> uiSize << " " << pFileInfo -> caTime;
                }
            }
            respdu->UiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_RESPOND;
            write((char *)respdu, respdu->uilPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DEL_DIR_REQUEST:{
            char caName[32] = {'\0'};
            strcpy(caName, pdu->caData);
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
            QString strPath = QString("%1/%2").arg(pPath).arg(caName);
            qDebug() << strPath;

            QFileInfo fileInfo(strPath);
            bool ret = false;
            if(fileInfo.isDir()){
                QDir dir;
                dir.setPath(strPath);
                ret = dir.removeRecursively();
            }else if(fileInfo.isFile()){
                ret = false;
            }
            PDU *respdu = NULL;
            if(ret){
                respdu = mkPDU(0);
                respdu->UiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
                memcpy(respdu->caData, DEL_DIR_OK, strlen(DEL_DIR_OK));
            }else{
                respdu = mkPDU(0);
                respdu->UiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
                memcpy(respdu->caData, DEL_DIR_FAILED, strlen(DEL_DIR_FAILED));
            }
            write((char *)respdu, respdu->uilPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_RENAME_FILE_REQUEST:{
            char caOldName[32] = {'\0'};
            char caNewName[32] = {'\0'};
            strncpy(caOldName, pdu->caData, 32);
            strncpy(caNewName, pdu->caData + 32, 32);

            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);

            QString strOldPath = QString("%1/%2").arg(pPath).arg(caOldName);
            QString strNewPath = QString("%1/%2").arg(pPath).arg(caNewName);

            qDebug() << "old path" << strOldPath;
            qDebug() << "new path" << strNewPath;

            QDir dir;
            bool ret = dir.rename(strOldPath, strNewPath);

            PDU *respdu = mkPDU(0);
            respdu->UiMsgType = ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
            if(ret){
                memcpy(respdu->caData, RENAME_FILE_OK, strlen(RENAME_FILE_OK));
            }else{
                memcpy(respdu->caData, RENAME_FILE_FAILED, strlen(RENAME_FILE_FAILED));
            }

            write((char *)respdu, respdu->uilPDULen);
            free(respdu);
            respdu = NULL;

            break;
        }
        case ENUM_MSG_TYPE_ENTRY_DIR_REQUEST:{
            char caEnterName[32] = {'\0'};
            strncpy(caEnterName, pdu->caData, 32);

            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);

            QString strPath = QString("%1/%2").arg(pPath).arg(caEnterName);

            QFileInfo fileInfo(strPath);

            PDU *respdu = NULL;

            if(fileInfo.isDir()){
                QDir dir;
                respdu = NULL;

                if(!dir.exists(strPath)) // 请求文件夹不存在
                {
                    respdu = mkPDU(0);
                    strncpy(respdu -> caData, PATH_NOT_EXIST, 32);
                }
                else // 存在
                {
                    dir.setPath(strPath); // 设置为当前目录
                    QFileInfoList fileInfoList = dir.entryInfoList(); // 获取当前目录下所有文件
                    int iFileNum = fileInfoList.size();

                    respdu = mkPDU(sizeof(FileInfo) * iFileNum);
                    FileInfo *pFileInfo = NULL; // 创建一个文件信息结构体指针，方便之后遍历PDU空间来赋值
                    strncpy(respdu -> caData, FLUSH_DIR_OK, 32);

                    for(int i = 0; i < iFileNum; ++ i)
                    {
                        pFileInfo = (FileInfo*)(respdu -> caMsg) + i; // 通过指针指向，直接修改PDU空间值，每次偏移FileInfo大小
                        memcpy(pFileInfo -> caName, fileInfoList[i].fileName().toStdString().c_str(), fileInfoList[i].fileName().size());
                        pFileInfo -> bIsDir = fileInfoList[i].isDir();
                        pFileInfo -> uiSize = fileInfoList[i].size();
                        QDateTime dtLastTime = fileInfoList[i].lastModified(); // 获取文件最后修改时间
                        QString strLastTime = dtLastTime.toString("yyyy/MM/dd hh:mm");
                        memcpy(pFileInfo -> caTime, strLastTime.toStdString().c_str(), strLastTime.size());
                        qDebug() << "文件信息：" << pFileInfo -> caName << " " << pFileInfo -> bIsDir << " " << pFileInfo -> uiSize << " " << pFileInfo -> caTime;
                    }
                }
                respdu->UiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_RESPOND;
                write((char *)respdu, respdu->uilPDULen);
                free(respdu);
                respdu = NULL;
            }else if(fileInfo.isFile()){
                respdu = mkPDU(0);
                respdu->UiMsgType = ENUM_MSG_TYPE_ENTRY_DIR_RESPOND;
                strncpy(respdu->caData, ENTER_DIR_FAILED, strlen(ENTER_DIR_FAILED));

                write((char *)respdu, respdu->uilPDULen);
                free(respdu);
                respdu = NULL;
            }

            break;
        }
        case ENUM_MSG_TYPE_DELETE_FILE_REQUEST:{
            char caName[32] = {'\0'};
            strcpy(caName, pdu->caData);
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
            QString strPath = QString("%1/%2").arg(pPath).arg(caName);
            qDebug() << strPath;

            QFileInfo fileInfo(strPath);
            bool ret = false;
            if(fileInfo.isDir()){
                ret = false;

            }else if(fileInfo.isFile()){
                QDir dir;
                ret = dir.remove(strPath);
            }
            PDU *respdu = NULL;
            if(ret){
                respdu = mkPDU(0);
                respdu->UiMsgType = ENUM_MSG_TYPE_DELETE_FILE_RESPOND;
                memcpy(respdu->caData, DEL_FILE_OK, strlen(DEL_FILE_OK));
            }else{
                respdu = mkPDU(0);
                respdu->UiMsgType = ENUM_MSG_TYPE_DELETE_FILE_RESPOND;
                memcpy(respdu->caData, DEL_FILE_FAILED, strlen(DEL_FILE_FAILED));
            }
            write((char *)respdu, respdu->uilPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:{
            char caFileName[32] = {'\0'};
            qint64 fileSize;
            sscanf(pdu->caData, "%s %lld", caFileName, &fileSize);
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
            QString strPath = QString("%1/%2").arg(pPath).arg(caFileName);
            qDebug() << strPath;

            delete [] pPath;
            pPath = NULL;

            m_file.setFileName(strPath);
            if(m_file.open(QIODevice::WriteOnly)){
                m_bUpload = true;
                m_iTotal = fileSize;
                m_iRecved = 0;
            }

            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:{
            char caFileName[32] = {'\0'};
            strcpy(caFileName, pdu->caData);
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
            QString strPath = QString("%1/%2").arg(pPath).arg(caFileName);
            qDebug() << strPath;

            delete [] pPath;
            pPath = NULL;

            QFileInfo fileinfo(strPath);
            qint64 filesize = fileinfo.size();
            PDU *respdu = mkPDU(0);
            respdu->UiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
            sprintf(respdu->caData, "%s %lld", caFileName, filesize);

            write((char *)respdu, respdu->uilPDULen);
            free(respdu);
            respdu = NULL;

            m_file.setFileName(strPath);
            m_file.open(QIODevice::ReadOnly);
            m_pTimer->start(1000);

            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:{
            char caSendName[32] = {'\0'};
            int num = 0;
            sscanf(pdu->caData, "%s%d", caSendName, &num);
            int size = num * 32;
            PDU *respdu = mkPDU(pdu->uiMsgLen - size);
            respdu->UiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE;
            strcpy(respdu->caData, caSendName);
            memcpy(respdu->caMsg, (char *)(pdu->caMsg) + size, pdu->uiMsgLen-size);

            char caRecvName[32] = {'\0'};
            for(int i = 0; i < num; ++i){
                memcpy(caRecvName, (char *)(pdu->caMsg) + i * 32, 32);
                mytcpser::getInstance().resend(caRecvName, respdu);
            }
            free(respdu);
            respdu = NULL;

            respdu = mkPDU(0);
            respdu->UiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
            strcpy(respdu->caData, "share file ok!");
            write((char *)respdu, respdu->uilPDULen);

            free(respdu);
            respdu = NULL;

            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND:{
            QString strRecvPath = QString("./%1").arg(pdu->caData);
            QString strShareFilePath = QString("%1").arg((char *)(pdu->caMsg));

            int index = strShareFilePath.lastIndexOf('/');
            QString strFileName = strShareFilePath.right(strShareFilePath.size()-index-1);
            strRecvPath = strRecvPath + '/' + strFileName;


            QFileInfo fileInfo(strShareFilePath);
            if(fileInfo.isFile()){
                QFile::copy(strShareFilePath, strRecvPath);
            }else if(fileInfo.isDir()){

                copyDir(strShareFilePath, strRecvPath);
            }
            break;
        }
        case ENUM_MSG_TYPE_MOVE_FILE_REQUEST:{
            char caFileName[32] = {'\0'};
            int srcLen = 0;
            int destLen = 0;
            sscanf(pdu->caData, "%d%d%s", &srcLen, &destLen, caFileName);

            char *pScrPath = new char[srcLen + 1];
            char *pDestPath = new char[destLen + 1 + 32];
            memset(pScrPath, '\0', srcLen + 1);
            memset(pDestPath, '\0', destLen + 1 + 32);

            memcpy(pScrPath, pdu->caMsg, srcLen);
            memcpy(pDestPath, (char *)(pdu->caMsg) + (srcLen + 1), destLen);

            PDU *respdu = mkPDU(0);
            respdu->UiMsgType = ENUM_MSG_TYPE_MOVE_FILE_RESPOND;
            QFileInfo fileInfo(pDestPath);
            if(fileInfo.isDir()){
                strcat(pDestPath, "/");
                strcat(pDestPath, caFileName);

                bool ret = QFile::rename(pScrPath, pDestPath);
                if(ret){
                    strcpy(respdu->caData, MOVE_FILE_OK);
                }else{
                    strcpy(respdu->caData, COMMON_ERR);
                }
            }else if(fileInfo.isFile()){
                strcpy(respdu->caData, MOVE_FILE_OK_FAILED);
            }
            write((char *)respdu, respdu->uilPDULen);
            free(respdu);
            respdu = NULL;

            break;
        }
        default:
            break;
        }
        free(pdu);
        pdu = NULL;
    }
    else{
        PDU *respdu = NULL;
        respdu = mkPDU(0);
        respdu->UiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
        QByteArray buff = readAll();
        m_file.write(buff);
        m_iRecved += buff.size();
        if(m_iRecved == m_iTotal){
            m_file.close();
            m_bUpload = false;

            strcpy(respdu->caData, UPLOAD_OK);

            write((char *)respdu, respdu->uilPDULen);
            free(respdu);
            respdu = NULL;
        }else if(m_iTotal < m_iRecved){
            m_file.close();
            m_bUpload = false;

            strcpy(respdu->caData, UPLOAD_FAILED);

            write((char *)respdu, respdu->uilPDULen);
            free(respdu);
            respdu = NULL;
        }

    }
}

void MyTcpSocket::clientOffline()
{
    OPeDb::getInstance().handleOffline(m_strName.toStdString().c_str());
    emit offline(this);
}

void MyTcpSocket::sendFileToClient()
{
    m_pTimer->stop();
    char *pData = new char[4096];
    qint64 ret = 0;
    while(true){
        ret = m_file.read(pData, 4096);
        if(ret > 0 && ret <= 4096){
            write(pData, ret);
        }else if(0 == ret){
            m_file.close();
            break;
        }
        else if(ret < 0){
            qDebug() << "发送数据时失败";
            m_file.close();
            break;
        }
    }
    m_file.close(); // 关闭文件
    delete [] pData;
    pData = NULL;
    m_file.setFileName("");
}
