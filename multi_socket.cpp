#include "multi_socket.h"

MultiSocket::MultiSocket(QTcpSocket* socket , QObject *parent) : QObject(parent),_socket(socket)
{
    if(_socket != nullptr)
    {
        connect(_socket,SIGNAL(readyRead()),this,SLOT(_socket_ready()));
    }
}

void MultiSocket::_socket_ready()
{
    emit readyRead(this);
}
