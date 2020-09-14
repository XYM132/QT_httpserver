#include "mainwindow.h"

#include <QApplication>
#include <QUuid>
#include <QMouseEvent>
#include "qrcodeshow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
//    w.show();

//    for(int i = 0 ;i< 10;i++)
//        qDebug() << QUuid::createUuid().toString().remove("{").remove("}").remove("-");

//    QByteArray byteArrayPercentEncoded = QByteArray::fromPercentEncoding("fdgfhgj");
//    qDebug() << byteArrayPercentEncoded.data();
//    QRcodeShow w((char*)"http://192.168.1.12:8080");

    w.show();
    return a.exec();
}
