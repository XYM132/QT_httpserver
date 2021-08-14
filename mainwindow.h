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
#include <QNetworkInterface>
#include <QVector>
#include <QListWidget>
#include <QMouseEvent>

#include "http_parse.h"
#include "webkitformboundary_parse.h"
#include "multi_socket.h"
#include "qrcodeshow.h"

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
    //void readyread();
    void getFile(MultiSocket* multi_socket);
    void new_connect();
    void multi_readyread(MultiSocket* multi_socket);

private slots:
    void on_pushButton_dir_clicked();

    void on_pushButton_file_clicked();

    void on_pushButton_listen_clicked();

    void on_pushButton_opendir_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_send_clicked();

//    void on_pushButton_dir_html_clicked();

    void on_listWidget_ip_itemPressed(QListWidgetItem *item);

    void mouseMoveEvent(QMouseEvent *p);

private:
    void append_data_show(QTextBrowser* tb,QString text,QString color,QString end = "\r\n\r\n");
    void check_v_multi_socket();

private:
    void file_deal(MultiSocket *multi_socket,QByteArray buf);


    QTcpServer* tcp_sever;
    QVector<MultiSocket*> v_multi_socket;

    QByteArray file_data;
    QByteArray part_data;

    WebKitFormBoundary_parse* webkit = NULL;
    http_parse* http_head = NULL;

    QFileInfo* file_info=NULL;

    QRcodeShow* codeShow=NULL;
    QPoint mousePos;

    QString send_str = "";
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
