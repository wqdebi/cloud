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

bool OPeDb::handleRegist(const char *Name, const char *Pwd)
{
    if(Name == nullptr || Pwd == nullptr)
        return false;
    QString str = QString("insert into usrInfo(name, pwd) values(\'%1\', \'%2\')").arg(Name).arg(Pwd);
    QSqlQuery query;
    return  query.exec(str);
}

bool OPeDb::handleLogin(const char *Name, const char *Pwd)
{
    if(Name == nullptr || Pwd == nullptr)
        return false;
    QString str = QString("select * from usrInfo where name=\'%1\' and pwd=\'%2\' and online=0").arg(Name).arg(Pwd);
    QSqlQuery query;
    query.exec(str);
    if(query.next()){
        QString str = QString("update usrInfo set online = 1 where name=\'%1\' and  pwd=\'%2\'").arg(Name).arg(Pwd);
        QSqlQuery query;
        query.exec(str);
        return true;
    }else{
        return false;
    }
}
