#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>

Book::Book(QWidget *parent) : QWidget(parent)
{
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
        QString strDelName = pItem->text().split('\t')[0];;
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




