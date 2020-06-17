#include "mainwindow.h"
#include "ui_mainwindow.h"

#define QT_NO_CAST_FROM_ASCII 1
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tcp_sever = new QTcpServer(this);

    file_info = new QFileInfo();

    time = new QTimer(this);
    time->setSingleShot(true);
    connect(time,SIGNAL(timeout()),this,SLOT(connect_timeout()));

    ui->pushButton_listen->click();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readyread()
{
    while(tcp_socket->bytesAvailable() > 0)
    {
        QByteArray buf = tcp_socket->readAll();
        http_head = new http_parse(QString(buf));

        QByteArray response;
        QString Content_Type = "text/html;charset=utf-8";
        QString additon = ""; //用于在http请求头中添加选项
        int status = 200;

        if(http_head->path == "/")
        {
            QFile html(QDir::currentPath() + "/../http_server/html/index.html");
            html.open(QIODevice::ReadOnly);
            response = html.readAll();
            html.close();

            status = 200;
        }
        else if(http_head->path == "/data")
        {
            append_data_show(ui->textBrowser_recive,http_head->http_content+"\r\n\r\n","#000");
            status = 200;
        }
        else if(http_head->path == "/file")
        {
            webkit = new WebKitFormBoundary_parse();
            part_data.clear();

            disconnect(tcp_socket,SIGNAL(readyRead()),this,SLOT(readyread()));
            connect(tcp_socket,SIGNAL(readyRead()),this,SLOT(getFile()));
            return;
        }
        else if(http_head->path == "/getFile")
        {
            if(file_info->fileName() == "")
            {
                additon += QString("Location: /\r\n");
            }
            else
            {
                additon += QString("Location: /getFile/%1\r\n").arg(file_info->fileName());
            }
            status = 302;
        }
        else if(http_head->path == "/getFile/"+file_info->fileName())
        {
            QFile file(file_info->filePath());
            file.open(QIODevice::ReadOnly);
            response = file.readAll();
            Content_Type = "application/octet-stream";
            file.close();
            status = 200;
        }
        else if(http_head->path == "/sendData")
        {
            if(send_str != "")
            {
                response = send_str.toUtf8();
                send_str = "";
            }

            status = 200;
        }
        else if(http_head->path == "/favicon.ico")
        {
            status = 404;
        }
        else
        {
            qDebug() << http_head->path;
            append_data_show(ui->textBrowser_info,QString(http_head->path) + "  404\r\n","#e00","\r\n");
            status = 404;
        }


        QString http = QString("HTTP/1.1 %1 OK\r\n").arg(status);
        http += "Server: nginx\r\n";
        http += QString("Content-Type: %1\r\n").arg(Content_Type);
        http += "Connection: keep-alive\r\n";
        http += additon;
        http += QString("Content-Length: %1\r\n\r\n").arg(QString::number(response.size()));

        tcp_socket->write(http.toUtf8());
        tcp_socket->write(response);
        tcp_socket->flush();
        tcp_socket->waitForBytesWritten(http.size() + response.size());
        tcp_socket->close();
    }
}

void MainWindow::getFile()
{
    time->stop();
    while(tcp_socket->bytesAvailable() > 0)
    {
        QByteArray buf = tcp_socket->readAll();
        if(webkit->boundary == "")
        {
            webkit = new WebKitFormBoundary_parse(QString(buf));

            buf = buf.mid(webkit->head_size);
            if(webkit->chunk == "0")
                append_data_show(ui->textBrowser_info,QString("reciving: ") + webkit->name + "...","#aac");
            ui->progressBar->setValue((webkit->chunk.toInt() + 1) * 100 / webkit->chunks.toInt());
        }

        part_data.append(buf);
        if(part_data.size() + webkit->head_size == this->http_head->Content_Length)
        {
            part_data = part_data.mid(0,part_data.length() - webkit->foot_size);
            file_data.append(part_data);

            QString response = "";
            if(webkit->chunk.toInt() == webkit->chunks.toInt() -1)
            {
                response = QString("{result:\'ok\',id:10001,url:\'\'}");

                QDir dir(ui->lineEdit_dir->text());
                QFile* target = new QFile(dir.absolutePath() + "/" + webkit->name);
                target->open(QIODevice::WriteOnly|QIODevice::Truncate);
                target->write(file_data);
                target->close();

                file_data.clear();
                ui->progressBar->reset();
                append_data_show(ui->textBrowser_recive,QString("recivefile:") + webkit->name,"#090");
            }
            else {

                response = QString("{\"args\":{},\"data\":\"\",\"files\":{\"file\":\"data:application/octet-stream;base64,\"},\"form\":{\"chunk\":\"%1\",\"chunks\":\"%2\",\"name\":\"%3\",\"uuid\":\"%4\"},\"headers\":{\"Accept\":\"*/*\",\"Accept-Encoding\":\"gzip,deflate,br\",\"Accept-Language\":\"zh-CN,zh;q=0.9,en-US;q=0.8,en;q=0.7\",\"Content-Length\":\"%5\",\"Content-Type\":\"multipart/form-data;boundary=----%6\",\"Sec-Fetch-Dest\":\"empty\",\"Sec-Fetch-Mode\":\"cors\",\"Sec-Fetch-Site\":\"cross-site\",\"User-Agent\":\"Mozilla/5.0(X11;Linuxx86_64)AppleWebKit/537.36(KHTML,likeGecko)Chrome/83.0.4103.97Safari/537.36\",\"X-Amzn-Trace-Id\":\"Root=\"},\"json\":null}")\
                        .arg(webkit->chunk).arg((webkit->chunks)).arg(webkit->name).arg(webkit->uuid).arg(part_data.size()).arg(webkit->boundary);
            }

            QString http = QString("HTTP/1.1 200 OK\r\n");
            http += "access-control-allow-credentials: true\r\n";
            http += "Content-Type: text/html;charset=utf-8\r\n";
            http += "content-type: application/json\r\n";
            http += "status: 200\r\n";
            http +=   QString("Content-Length: %1\r\n\r\n").arg(QString::number(response.size()));

            tcp_socket->write(http.toUtf8());
            tcp_socket->write(response.toUtf8());
            tcp_socket->flush();
            tcp_socket->waitForBytesWritten(http.size() + response.size());
            tcp_socket->close();
        }
        else
        {
            time->start(2000);
        }
    }
}

void MainWindow::new_connect()
{
    while (tcp_sever->hasPendingConnections())
    {
        if(tcp_socket != nullptr)
            tcp_socket->close();
        tcp_socket = tcp_sever->nextPendingConnection();
        connect(tcp_socket,SIGNAL(readyRead()),this,SLOT(readyread()));
    }
}

void MainWindow::connect_timeout()
{
    tcp_socket->close();
}


void MainWindow::on_pushButton_dir_clicked()
{
    QString dir_name = QFileDialog::getExistingDirectory(
                this,
                tr("Open Directory"),
                "./");
    if(dir_name.size() != 0)
        ui->lineEdit_dir->setText(dir_name);

}

void MainWindow::on_pushButton_file_clicked()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Open file"),
                "./");
    if(filename.size() != 0)
    {
        ui->lineEdit_file->setText(filename);
        file_info = new QFileInfo(filename);
    }
}

void MainWindow::on_pushButton_listen_clicked()
{
    if(ui->pushButton_listen->text() == "监听")
    {
        int port = ui->spinBox_port->value();
        if(tcp_sever->listen(QHostAddress::Any,port) == true)
        {
            connect(tcp_sever,SIGNAL(newConnection()),this,SLOT(new_connect()));
            ui->pushButton_listen->setText("停止监听");
        }
    }
    else {
        tcp_sever->close();
        ui->pushButton_listen->setText("监听");
    }
}

void MainWindow::on_pushButton_opendir_clicked()
{
    QDir dir(ui->lineEdit_dir->text());
    QDesktopServices::openUrl(QUrl(dir.absolutePath(), QUrl::TolerantMode));
}

void MainWindow::append_data_show(QTextBrowser* tb, QString text,QString color,QString end/* = \r\n\r\n*/)
{
    QTime current_time =QTime::currentTime();
    QString time = current_time.toString("hh:mm:ss");
    tb->append(QString("<font color='%1'>[%2] %3</font>%4").arg(color).arg(time).arg(text).arg(end));
}

void MainWindow::on_pushButton_clear_clicked()
{
    ui->textEdit_send->clear();
}

void MainWindow::on_pushButton_send_clicked()
{
    send_str = ui->textEdit_send->toPlainText();
}
