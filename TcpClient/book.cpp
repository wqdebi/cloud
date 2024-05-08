#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "sharefile.h"
#include "opewidget.h"

Book::Book(QWidget *parent) : QWidget(parent)
{
    m_strEnterDir.clear();

    m_pTimer = new QTimer;

    m_bDownload = false;

    m_pBoolListW = new QListWidget;
    m_pReturnPB = new QPushButton("返回");
    m_pCreateDirPB = new QPushButton("创建文件夹");
    m_pDelDirPB = new QPushButton("删除文件夹");
    m_pRenamePB = new QPushButton("重命名文件夹");
    m_pFlushFilePB = new QPushButton("刷新文件");

    QVBoxLayout *pDirVBL = new QVBoxLayout;
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDelDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushFilePB);

    m_pUploadPB = new QPushButton("上传文件");
    m_pDownloadPB = new QPushButton("下载文件");
    m_pDelFilePB = new QPushButton("删除文件");
    m_pShareFilePB = new QPushButton("共享文件");

    QVBoxLayout *pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownloadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFilePB);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pBoolListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    connect(m_pCreateDirPB, SIGNAL(clicked(bool)),
            this, SLOT(createDir()));

    connect(m_pFlushFilePB, SIGNAL(clicked(bool)),
            this, SLOT(flushFile()));

    connect(m_pDelDirPB, SIGNAL(clicked(bool)),
            this, SLOT(delDir()));

    connect(m_pRenamePB, SIGNAL(clicked(bool)),
            this, SLOT(renameFile()));

    connect(m_pBoolListW, SIGNAL(doubleClicked(QModelIndex)),
                this, SLOT(entryDir(QModelIndex)));

    connect(m_pReturnPB, SIGNAL(clicked(bool)),
            this, SLOT(returnPre()));

    connect(m_pDelFilePB, SIGNAL(clicked(bool)),
            this, SLOT(delRegFile()));

    connect(m_pUploadPB, SIGNAL(clicked(bool)),
            this, SLOT(uploadFile()));

    connect(m_pTimer, SIGNAL(timeout()),
            this, SLOT(upLoadFileData()));

    connect(m_pDownloadPB, SIGNAL(clicked(bool)),
            this, SLOT(downloadFile()));
    connect(m_pShareFilePB, SIGNAL(clicked(bool)),
            this, SLOT(shareFile()));

}

void Book::updateFileList(const PDU *pdu)
{
    if(NULL == pdu)
    {
        return ;
    }
    uint uiFileNum = pdu -> uiMsgLen / sizeof(FileInfo); // 文件数
    FileInfo *pFileInfo = NULL; // 通过FileInfo指针依此访问caMsg中数据
    QListWidgetItem *pItem = NULL;

    m_pBoolListW -> clear(); // 清除文件列表原有数据
    for(uint i = 0; i < uiFileNum; ++ i)
    {
        pFileInfo = (FileInfo*)(pdu -> caMsg) + i;
        if(strcmp(pFileInfo -> caName, ".") == 0 || strcmp(pFileInfo -> caName, "..") == 0)
        {   // 不显示 "." ".." 文件夹
            continue;
        }
        pItem = new QListWidgetItem;

        if(pFileInfo ->bIsDir) // 根据文件类型设置图标
        {
            pItem->setIcon(QIcon(QPixmap(":/dir.jpeg")));
        }
        else
        {
            pItem->setIcon(QIcon(QPixmap(":/file.jpeg")));
        }
        // 文件名 文件大小 最后修改时间  形式展示文件
        pItem ->setText(QString("%1\t%2\t%3").arg(pFileInfo->caName)
                        .arg(pFileInfo->uiSize).arg(pFileInfo->caTime));
        m_pBoolListW->addItem(pItem);
    }
}

void Book::clearEnterDir()
{
    m_strEnterDir.clear();
}

QString Book::enterDir()
{
    return m_strEnterDir;
}

void Book::setDownloadStatus(bool s)
{
    m_bDownload = s;
}

bool Book::getDownloadStatus()
{
    return m_bDownload;
}

QString Book::getSaveFilePath()
{
    return m_strSaveFilePath;
}

QString Book::getShareFileName()
{
    return m_strShareFileName;
}

void Book::createDir()
{
    QString strNewDir = QInputDialog::getText(this, "新建文件夹", "新文件名字");
    if(!strNewDir.isEmpty()){
        if(strNewDir.size() > 32){
            QMessageBox::warning(this, "新建文件夹", "新文件夹名字不能超过32个字符");
        }else{
            QString strName = TcpClient::getinstance().loginName();
            QString strCurPath = TcpClient::getinstance().curPath();
            qDebug() << "strCurPath" << strCurPath;
            PDU *pdu = mkPDU(strCurPath.size() + 1);
            pdu->UiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            strncpy(pdu->caData, strName.toStdString().c_str(), strName.size());
            strncpy(pdu->caData + 32, strNewDir.toStdString().c_str(), strNewDir.size());
            memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
            TcpClient::getinstance().getTcpSocket().write((char *)pdu, pdu->uilPDULen);
            free(pdu);
            pdu = NULL;
        }
    }else{
        QMessageBox::warning(this, "新建文件夹", "不能为空");
    }

}

void Book::flushFile()
{
    QString strCurPath = TcpClient::getinstance().curPath();
    PDU *pdu = mkPDU(strCurPath.size() + 1);
    pdu->UiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_REQUEST;
    strncpy((char *)pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
    TcpClient::getinstance().getTcpSocket().write((char *)(pdu), pdu->uilPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::delDir()
{
    QString strCurPath = TcpClient::getinstance().curPath();

    QListWidgetItem *pItem = m_pBoolListW->currentItem();
    if(NULL == pItem){
        QMessageBox::warning(this, "删除文件", "请选择要删除的文件");
    }else{
        QString strDelName = pItem->text().split('\t')[0];
        qDebug() << strDelName;

        PDU *pdu = mkPDU(strCurPath.size() + 1);
        pdu->UiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;
        strncpy(pdu->caData, strDelName.toStdString().c_str(), strDelName.size());
        memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
        TcpClient::getinstance().getTcpSocket().write((char *)(pdu), pdu->uilPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void Book::renameFile()
{
    QString strCurPath = TcpClient::getinstance().curPath();
    QListWidgetItem *pItem = m_pBoolListW->currentItem();
    if(NULL == pItem){
        QMessageBox::warning(this, "重命名文件", "请选择要重命名的文件");
    }else{
        QString strOldName = pItem->text().split('\t')[0];
        QString strNewName = QInputDialog::getText(this, "重命名文件", "请输入新文件名");
        if(!strNewName.isEmpty()){
            PDU *pdu = mkPDU(strCurPath.size() + 1);
            pdu->UiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
            strncpy(pdu->caData, strOldName.toStdString().c_str(), strOldName.size());
            strncpy(pdu->caData + 32, strNewName.toStdString().c_str(), strNewName.size());
            memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
            TcpClient::getinstance().getTcpSocket().write((char *)(pdu), pdu->uilPDULen);
            free(pdu);
            pdu = NULL;
        }else{
            QMessageBox::warning(this, "重命名文件", "新文件名不能为空");
        }
    }
}

void Book::entryDir(const QModelIndex &index)
{
    QString strDirName = index.data().toString().split('\t')[0];
    m_strEnterDir = strDirName;
    QString strCurPath = TcpClient::getinstance().curPath();
    PDU *pdu = mkPDU(strCurPath.size() + 1);
    pdu->UiMsgType = ENUM_MSG_TYPE_ENTRY_DIR_REQUEST;
    strncpy(pdu->caData, strDirName.toStdString().c_str(), strDirName.size());
    memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());

    TcpClient::getinstance().getTcpSocket().write((char *)(pdu), pdu->uilPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::returnPre()
{
    QString strCurPath = TcpClient::getinstance().curPath();
    QString strRootPath = "./" + TcpClient::getinstance().loginName();
    if(strCurPath == strRootPath){
        QMessageBox::warning(this, "返回上一级", "返回失败");
    }else{
        int index = strCurPath.lastIndexOf('/');
        strCurPath.remove(index, strCurPath.size() - index);
        qDebug() << "return: " << strCurPath;
        TcpClient::getinstance().setCurPath(strCurPath);

        clearEnterDir();

        flushFile();
    }
}

void Book::delRegFile()
{
    QString strCurPath = TcpClient::getinstance().curPath();

    QListWidgetItem *pItem = m_pBoolListW->currentItem();
    if(NULL == pItem){
        QMessageBox::warning(this, "删除文件", "请选择要删除的文件");
    }else{
        QString strDelName = pItem->text().split('\t')[0];
        qDebug() << strDelName;

        PDU *pdu = mkPDU(strCurPath.size() + 1);
        pdu->UiMsgType = ENUM_MSG_TYPE_DELETE_FILE_REQUEST;
        strncpy(pdu->caData, strDelName.toStdString().c_str(), strDelName.size());
        memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
        TcpClient::getinstance().getTcpSocket().write((char *)(pdu), pdu->uilPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void Book::uploadFile()
{

    m_strUploadFilePath = QFileDialog::getOpenFileName();
    qDebug() << m_strUploadFilePath;

    if(!m_strUploadFilePath.isEmpty()){
        int index = m_strUploadFilePath.lastIndexOf('/');
        QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size() - index - 1);
        qDebug() << "strFileName: " << strFileName;

        QFile file(m_strUploadFilePath);
        quint64 fileSize = file.size();

        QString strCurPath = TcpClient::getinstance().curPath();
        PDU *pdu = mkPDU(strCurPath.size() + 1);
        pdu->UiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
        sprintf(pdu->caData, "%s %lld", strFileName.toStdString().c_str(), fileSize);

        TcpClient::getinstance().getTcpSocket().write((char *)(pdu), pdu->uilPDULen);
        free(pdu);
        pdu = NULL;

        m_pTimer->start(1000);
    }else{
        QMessageBox::warning(this, "上传文件", "上传文件不能为空");
    }
}

void Book::upLoadFileData()
{
    m_pTimer->stop();
    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this, "上传文件", "上传文件失败");
        return;
    }

    char *pBuffer = new char[4096];
    qint64 ret = 0;
    while(true){
        ret = file.read(pBuffer, 4096);
        if(ret > 0 && ret <= 4096){
            TcpClient::getinstance().getTcpSocket().write(pBuffer, ret);
        }else if(0 == ret){
            break;
        }else{
            QMessageBox::warning(this, "上传文件", "上传文件失败：读取失败。");
            break;
        }
    }
    file.close();
    delete [] pBuffer;
}

void Book::downloadFile()
{
    QListWidgetItem *pItem = m_pBoolListW->currentItem();
    if(NULL == pItem){
        QMessageBox::warning(this, "下载文件", "请选择要下载的文件");
    }else{
       QString strCurPath = TcpClient::getinstance().curPath();
       PDU *pdu = mkPDU(strCurPath.size() + 1);
       pdu->UiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
       QString strFileName = pItem->text().split('\t')[0];
       strcpy(pdu->caData, strFileName.toStdString().c_str());
       memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());

       QString strSaveFilePath = QFileDialog::getSaveFileName();
       if(strSaveFilePath.isEmpty()){
           QMessageBox::warning(this, "下载文件", "保存路径不能为空");
           m_strSaveFilePath.clear();
       }else{
           m_strSaveFilePath = strSaveFilePath;
       }

       TcpClient::getinstance().getTcpSocket().write((char *)(pdu), pdu->uilPDULen);
       free(pdu);
       pdu = NULL;
    }
}

void Book::shareFile()
{
    QListWidgetItem *pItem = m_pBoolListW->currentItem();
    if(NULL == pItem){
        QMessageBox::warning(this, "分享文件", "请选择要分享的文件");
        return;
    }else{
        m_strShareFileName = pItem->text().split('\t')[0];
    }
    Friend *pfriend = OpeWidget::getInstance().getFriend();
    QListWidget *pFriendList = pfriend->getFriendList();

//    for(int i = 0; i < pFriendList->count(); ++i){
//        qDebug() << pFriendList->item(i)->text();
//    }
    ShareFile::getInstance().updateFiend(pFriendList);
    if(ShareFile::getInstance().isHidden()){
        ShareFile::getInstance().show();
    }
}








