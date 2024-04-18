#ifndef OPEDB_H
#define OPEDB_H

#include <QSqlDatabase>


#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

class OPeDb : public QObject
{
    Q_OBJECT
private:
    explicit OPeDb(QObject *parent = nullptr);
public:
    void init();
    static OPeDb& getInstance();


signals:

private:
    QSqlDatabase m_db;

};

#endif // OPEDB_H
