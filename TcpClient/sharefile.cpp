#include "sharefile.h"
#include <QCheckBox>
#include "tcpclient.h"
#include "opewidget.h"

ShareFile::ShareFile(QWidget *parent) : QWidget(parent)
{
    m_pSelectAllPB = new QPushButton("全选");
    m_pCancleSelectPB = new QPushButton("取消全选");

    m_pOkPB=  new QPushButton("确定");
    m_pCanclePB = new QPushButton("取消");

    m_pSA = new QScrollArea;
    m_pFriendsW = new QWidget;

    m_pFriendWVBL = new QVBoxLayout(m_pFriendsW);

    m_pButtonGroup = new QButtonGroup(m_pFriendsW);
    m_pButtonGroup->setExclusive(false);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pSelectAllPB);
    pTopHBL->addWidget(m_pCancleSelectPB);
    pTopHBL->addStretch();

    QHBoxLayout *pDownHBL = new QHBoxLayout;
    pDownHBL->addWidget(m_pOkPB);
    pDownHBL->addWidget(m_pCanclePB);

    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL->addLayout(pTopHBL);
    pMainVBL->addWidget(m_pSA);
    pMainVBL->addLayout(pDownHBL);

    setLayout(pMainVBL);

    connect(m_pCancleSelectPB, SIGNAL(clicked(bool)),
            this, SLOT(cancleSelect()));

    connect(m_pSelectAllPB, SIGNAL(clicked(bool)),
            this, SLOT(selectAll()));

    connect(m_pOkPB, SIGNAL(clicked(bool)),
            this, SLOT(okShare()));

    connect(m_pCanclePB, SIGNAL(clicked(bool)),
            this, SLOT(cancelShare()));
}

ShareFile &ShareFile::getInstance()
{
    static ShareFile instance;
    return instance;
}

void ShareFile::updateFiend(QListWidget *pFriendList)
{
    if(NULL == pFriendList){
        return ;
    }
    QAbstractButton * tmp = NULL;
    QList<QAbstractButton *> preFriendList = m_pButtonGroup->buttons();
    for(int i = 0; i < preFriendList.size(); ++i){
        tmp = preFriendList[i];
        m_pFriendWVBL->removeWidget(tmp);
        m_pButtonGroup->removeButton(tmp);
        //preFriendList.removeOne(tmp);
        delete tmp;
        tmp = NULL;
    }

    QCheckBox *pCB = NULL;
    for(int i = 0; i < pFriendList->count(); ++i){
        pCB = new QCheckBox(pFriendList->item(i)->text());
        m_pFriendWVBL->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }
    m_pSA->setWidget(m_pFriendsW);
}

void ShareFile::selectAll()
{
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    for(int i = 0; i < cbList.size(); ++i){
        if(!cbList[i]->isChecked()){
            cbList[i]->setChecked(true);
        }
    }
}

void ShareFile::cancleSelect()
{
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    for(int i = 0; i < cbList.size(); ++i){
        if(cbList[i]->isChecked()){
            cbList[i]->setChecked(false);
        }
    }
}

void ShareFile::okShare()
{
    QString strName = TcpClient::getinstance().loginName();
    QString strCurPath = TcpClient::getinstance().curPath();
    QString strShareFileName = OpeWidget::getInstance().getBook()->getShareFileName();
    QString strPath = strCurPath + "/" + strShareFileName;

    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    int num = 0;
    for(int i = 0; i < cbList.size(); ++i){
        if(cbList[i]->isChecked()){
            ++num;
        }
    }

    PDU *pdu = mkPDU(32 * num + 1 + strPath.size());
    pdu->UiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    sprintf(pdu->caData, "%s, %d", strName.toStdString().c_str(), num);

    int j = 0;
    for(int i = 0; i < cbList.size(); ++i){
        if(cbList[i]->isChecked()){
            memcpy((char *)(pdu->caMsg) + j * 32, cbList[i]->text().toStdString().c_str(), 32);
            ++j;
        }
    }
    memcpy((char *)(pdu->caMsg) + num * 32, strPath.toStdString().c_str(), strPath.size());
    TcpClient::getinstance().getTcpSocket().write((char *)pdu, pdu->uilPDULen);

    free(pdu);
    pdu = NULL;
}

void ShareFile::cancleShare()
{
    this -> hide();
}
