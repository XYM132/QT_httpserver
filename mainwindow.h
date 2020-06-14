#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <qdebug.h>
#include <QFile>
#include <QDir>
#include <QTimer>

#include "http_parse.h"
#include "webkitformboundary_parse.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void readyread();
    void getFile();
    void new_connect();
    void connect_timeout();

private:
    QTcpServer* tcp_sever;
    QTcpSocket* tcp_socket = nullptr;
    QByteArray file_data;
    QByteArray part_data;
    WebKitFormBoundary_parse* webkit = nullptr;
    http_parse* http_head = nullptr;

    QString filename="opencv-4.3.0.tar.gz";
    QTimer *time;
    int num = 0;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
