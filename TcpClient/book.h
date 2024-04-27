#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include "protocal.h"
#include <QModelIndex>
#include <QTimer>

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(const PDU *pdu);

    void clearEnterDir();

    QString enterDir();

    void setDownloadStatus(bool s);

    qint64 m_iTotal;
    qint64 m_iRecved;

    bool getDownloadStatus();
    QString getSaveFilePath();

public slots:
    void createDir();
    void flushFile();
    void delDir();
    void renameFile();
    void entryDir(const QModelIndex &index);
    void returnPre();
    void delRegFile();
    void uploadFile();
    void upLoadFileData();
    void downloadFile();

signals:

private:
    QListWidget *m_pBoolListW;
    QPushButton* m_pReturnPB;
    QPushButton* m_pCreateDirPB;
    QPushButton* m_pDelDirPB;
    QPushButton* m_pRenamePB;
    QPushButton* m_pFlushFilePB;
    QPushButton* m_pUploadPB;
    QPushButton* m_pDownloadPB;
    QPushButton* m_pDelFilePB;
    QPushButton* m_pShareFilePB;

    QString m_strEnterDir;
    QString m_strUploadFilePath;

    QTimer *m_pTimer;

    QString m_strSaveFilePath;
    bool m_bDownload;
};

#endif // BOOK_H
