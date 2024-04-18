#include "opedb.h"
#include <QMessageBox>
#include <QDebug>

OPeDb::OPeDb(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

void OPeDb::init()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("D:/cloud/TcpServer/cloud.db");
    if(m_db.open()){
        QSqlQuery query;
        query.exec("select * from usrInfo");
        while(query.next())
        {
            QString data = QString("%1, %2, %3, %4").arg(query.value(0).toString()).arg(query.value(1).toString())
                    .arg(query.value(2).toString()).arg(query.value(3).toString());
            qDebug() << data;
        }
    }else{
        QMessageBox::critical(NULL, "打开数据库", "打开数据库失败");
    }

}

OPeDb &OPeDb::getInstance()
{
    static OPeDb instance;
    return instance;
}

OPeDb::~OPeDb()
{
    m_db.close();
}
