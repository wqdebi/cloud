#ifndef MYTCPSER_H
#define MYTCPSER_H
#include <QTcpServer>


class mytcpser : public QTcpServer
{
    Q_OBJECT
private:
    mytcpser();
public:
    static mytcpser& getInstance();

    void incomingConnection(qintptr handle) override;
};

#endif // MYTCPSER_H
