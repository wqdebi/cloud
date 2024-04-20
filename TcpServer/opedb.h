#ifndef OPEDB_H
#define OPEDB_H

#include <QSqlDatabase>


#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>

class OPeDb : public QObject
{
    Q_OBJECT
private:
    explicit OPeDb(QObject *parent = nullptr);
public:
    void init();
    static OPeDb& getInstance();
    ~OPeDb();
    bool handleRegist(const char *Name, const char *Pwd);
    bool handleLogin(const char *Name, const char *Pwd);
    void handleOffline(const char *Name);
    QStringList handleAllOnline();
    int handleSearchUsr(const char *name);
    int handleAddFriend(const char *perName, const char *Name);
    bool handleAddFriendAgree(const char *addedName, const char *sourceName);
    int getIdByUserName(const char *name);                // 根据用户名获取用户id
    QStringList handleFlushFriend(const char *name);

signals:

private:
    QSqlDatabase m_db;

};

#endif // OPEDB_H
