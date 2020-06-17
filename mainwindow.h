#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <qdebug.h>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QFileDialog>
#include <QFileInfo>
#include <QDesktopServices>
#include <QTextBrowser>
#include <QTime>

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

private slots:
    void on_pushButton_dir_clicked();

    void on_pushButton_file_clicked();

    void on_pushButton_listen_clicked();

    void on_pushButton_opendir_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_send_clicked();

private:
    void append_data_show(QTextBrowser* tb,QString text,QString color,QString end = "\r\n\r\n");

private:
    QTcpServer* tcp_sever;
    QTcpSocket* tcp_socket = nullptr;

    QByteArray file_data;
    QByteArray part_data;

    WebKitFormBoundary_parse* webkit = nullptr;
    http_parse* http_head = nullptr;

    QFileInfo* file_info=nullptr;

    QTimer *time;

    QString send_str = "";
    int num = 0;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
