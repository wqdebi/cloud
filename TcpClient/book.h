#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include "protocal.h"
#include <QModelIndex>

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(const PDU *pdu);

    void clearEnterDir();

    QString enterDir();

public slots:
    void createDir();
    void flushFile();
    void delDir();
    void renameFile();
    void entryDir(const QModelIndex &index);


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

};

#endif // BOOK_H
