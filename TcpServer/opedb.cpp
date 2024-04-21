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

void OPeDb::handleOffline(const char *Name)
{
    if(Name == nullptr)
        return;
    QString str = QString("update usrInfo set online = 0 where name=\'%1\'").arg(Name);
    QSqlQuery query;
    query.exec(str);
}

QStringList OPeDb::handleAllOnline()
{
    QString str = QString("select name from usrInfo where online = 1");
    QSqlQuery query;
    query.exec(str);
    QStringList result;
    result.clear();
    while(query.next()){
        result.append(query.value(0).toString());
    }
    return result;
}

int OPeDb::handleSearchUsr(const char *name)
{
    if(NULL == name){
        return -1;
    }
    QString data = QString("select * from usrInfo where name = '\%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
    if(query.next()){
        int ret = query.value(0).toInt();
        if(ret == 1){
            return 1;
        }else{
            return 0;
        }
    }else{
        return -1;
    }
}

int OPeDb::handleAddFriend(const char *perName, const char *Name)
{
    if(NULL == perName || Name == NULL){
        return -1;
    }
    QString data = QString("select * from friend where (id = (select id from usrInfo where name=\'%1\') and friendId = (select id from usrInfo where name = \'%2\')) "
                           "or (id = (select id from usrInfo where name=\'%3\') and friendId = (select id from usrInfo where name = \'%4\'))").arg(perName).arg(Name).arg(Name).arg(perName);
    QSqlQuery query;
    query.exec(data);
    if(query.next()){
        return 0;
    }else{
        QString data = QString("select online from usrInfo where name = '\%1\'").arg(perName);
        QSqlQuery query1;
        query1.exec(data);
        if(query1.next()){
            int ret = query1.value(0).toInt();
            if(ret == 1){
                return 1;
            }else{
                return 2;
            }
        }else{
            return 3;
        }
    }
}

bool OPeDb::handleAddFriendAgree(const char *addedName, const char *sourceName)
{
    if(NULL == addedName || NULL == sourceName)
    {
        qDebug() << "handleAddFriendAgree: name is NULL";
        return false;
    }

    int sourceUserId = -1, addedUserId = -1;
    // 查找用户对应id
    addedUserId = getIdByUserName(addedName);
    sourceUserId = getIdByUserName(sourceName);

    QString strQuery = QString("insert into friend values(%1, %2) ").arg(sourceUserId).arg(addedUserId);
    QSqlQuery query;

    qDebug() << "handleAddFriendAgree " << strQuery;

    return query.exec(strQuery);
}

int OPeDb::getIdByUserName(const char *name)
{
    if(NULL == name)
    {
        return -1;
    }
    QString strQuery = QString("select id from usrInfo where name = \'%1\' ").arg(name);
    QSqlQuery query;

    query.exec(strQuery);
    if(query.next())
    {
        return query.value(0).toInt();
    }
    else
    {
        return -1; // 不存在该用户
    }
}

QStringList OPeDb::handleFlushFriend(const char *name)
{
    QStringList strFriendList;
    strFriendList.clear();
    if(NULL == name){
        return strFriendList;
    }
    QString data = QString("select name from usrInfo where online = 1 and id in (select id from friend where friendId = (select id from usrInfo where name = \'%1\'))").arg(name);
    QSqlQuery query;
    query.exec(data);
    while(query.next()){
        strFriendList.append(query.value(0).toString());
        qDebug() << query.value(0).toString();
    }
    data = QString("select name from usrInfo where online = 1 and id in (select friendId from friend where id = (select id from usrInfo where name = \'%1\'))").arg(name);
    query.exec(data);
    while(query.next()){
        strFriendList.append(query.value(0).toString());
        qDebug() << query.value(0).toString();
    }
    return strFriendList;
}

bool OPeDb::handleDelFriend(const char *perName, const char *Name)
{
    if(NULL == perName || NULL == Name){
        return false;
    }
    QString data = QString("delete from friend where id = (select id from usrInfo where name = '\%1\') and friendId = (select id from usrInfo where name = '\%2\')").arg(Name).arg(perName);
    QSqlQuery query;
    query.exec(data);
    data = QString("delete from friend where id = (select id from usrInfo where name = '\%1\') and friendId = (select id from usrInfo where name = '\%2\')").arg(perName).arg(Name);
    query.exec(data);
    return true;
}
