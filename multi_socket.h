#ifndef MULTI_SOCKET_H
#define MULTI_SOCKET_H

#include <QObject>
#include <QTcpSocket>

#include <QDebug>
class MultiSocket : public QObject
{
    Q_OBJECT
public:
    explicit MultiSocket(QTcpSocket* socket ,QObject *parent = nullptr);
    QTcpSocket* _socket = nullptr;

signals:
    void readyRead(MultiSocket*);

public slots:
    void _socket_ready();
};

#endif // MULTI_SOCKET_H
