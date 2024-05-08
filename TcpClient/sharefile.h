#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QScrollArea>
#include <QListWidget>

class ShareFile : public QWidget
{
    Q_OBJECT
private:
    explicit ShareFile(QWidget *parent = nullptr);

public:
    static ShareFile &getInstance();

    void updateFiend(QListWidget *pFriendList);


public slots:
    void selectAll(); // 全选
    void cancleSelect(); // 取消所有选择
    void okShare(); // 确定分享
    void cancleShare(); // 取消分享
private:
    QPushButton *m_pSelectAllPB; // 全选
    QPushButton *m_pCancleSelectPB; // 取消选择

    QPushButton *m_pOkPB; // 确认键
    QPushButton *m_pCanclePB; // 取消键

    QScrollArea *m_pSA; // 展示好友区
    QWidget *m_pFriendsW; // 所有好友窗口
    QVBoxLayout *m_pFriendsVBL; // 好友信息垂直布局
    QButtonGroup *m_pButtonGroup;

    QVBoxLayout *m_pFriendWVBL;

signals:

};

#endif // SHAREFILE_H
