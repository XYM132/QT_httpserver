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

    ui->pushButton_listen->click();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::multi_readyread(MultiSocket* multi_socket)
{
    QTcpSocket* tcp_socket = multi_socket->_socket;
    while(tcp_socket->bytesAvailable() > 0)
    {
        QByteArray buf = tcp_socket->readAll();
        http_head = new http_parse(buf);

        QByteArray response;
        QString Content_Type = "text/html;charset=utf-8";
        QString additon = ""; //用于在http请求头中添加选项
        int status = 200;

        if(http_head->path == "/")
        {
//            QString path = ui->lineEdit_dir_html->text();
//            QFileInfo htmlPath;
//            if(path.indexOf(".") == 0)
//                htmlPath = QFileInfo(QDir::currentPath() +"/"+path);
//            else
//                htmlPath = QFileInfo(path);

//            QFile html(htmlPath.absoluteFilePath() + "/index.html");
            QFile html(":/statics/html/index.html");
            if(html.open(QIODevice::ReadOnly))
            {
                response = html.readAll();
                html.close();

                status = 200;
            }
            else {
                append_data_show(ui->textBrowser_info,"无法打开 /index.html 文件，请在源文件路径中选择正确的路径","#f00");
                status = 404;
            }
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

            disconnect(multi_socket,SIGNAL(readyRead(MultiSocket*)),this,SLOT(multi_readyread(MultiSocket*)));
            connect(multi_socket,SIGNAL(readyRead(MultiSocket*)),this,SLOT(getFile(MultiSocket*)));

            if(http_head->http_content.length() != 0)
            {
                file_deal(multi_socket , http_head->http_content);
            }
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
        else if(QByteArray::fromPercentEncoding(http_head->path.toUtf8()).data() == "/getFile/"+file_info->fileName())
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
        else if(http_head->path.indexOf("/js_css") == 0)
        {
//            QString path = ui->lineEdit_dir_html->text();
//            QFileInfo htmlPath;
//            if(path.indexOf(".") == 0)
//                htmlPath = QFileInfo(QDir::currentPath() +"/"+path);
//            else
//                htmlPath = QFileInfo(path);

            QFile html(":/statics/html/" + http_head->path.mid(7));
            if(http_head->path.mid(7).indexOf("js") != -1)
                Content_Type = "application/javascript; charset=utf-8";
            else if(http_head->path.mid(7).indexOf("css") != -1)
                Content_Type = "text/css";
            if(html.open(QIODevice::ReadOnly))
            {
                response = html.readAll();
                html.close();

                status = 200;
            }
            else {
                append_data_show(ui->textBrowser_info,"无法打开"+ http_head->path.mid(7) + "文件，请在源文件路径中选择正确的路径","#f00");
                status = 404;
            }
        }
        else if(http_head->path == "/fonts/zenicon.woff?v=2.2.0")
        {
            Content_Type = "application/octet-stream";

            QFile html(":/statics/html/zenicon.woff");

            if(html.open(QIODevice::ReadOnly))
            {
                response = html.readAll();
                html.close();

                status = 200;
            }
            else {
                append_data_show(ui->textBrowser_info,"无法打开" + http_head->path.mid(7) + "文件，请在源文件路径中选择正确的路径","#f00");
                status = 404;
            }


        }
        else if(http_head->path == "/favicon.ico")
        {
            status = 404;
        }
        else
        {
            qDebug() << "path:" << http_head->path;
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


        delete multi_socket;
        multi_socket = nullptr;
        check_v_multi_socket();
    }
}


void MainWindow::file_deal(MultiSocket *multi_socket,QByteArray buf)
{
    QTcpSocket* tcp_socket = multi_socket->_socket;
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
            if(!dir.exists())
                dir.mkpath(".");
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

        delete multi_socket;
        multi_socket = nullptr;
        check_v_multi_socket();

    }
}

void MainWindow::getFile(MultiSocket *multi_socket)
{
    //time->stop();

    QTcpSocket* tcp_socket = multi_socket->_socket;
    while(tcp_socket->bytesAvailable() > 0)
    {
        QByteArray buf = tcp_socket->readAll();
        file_deal(multi_socket,buf);
    }
}

void MainWindow::check_v_multi_socket()
{
    for(int i = 0;i < v_multi_socket.size();i++)
    {
        if(v_multi_socket[i] == nullptr)
            v_multi_socket.erase(v_multi_socket.begin() + i);
    }
}


void MainWindow::new_connect()
{
    while (tcp_sever->hasPendingConnections())
    {
        QTcpSocket* tcp_socket = tcp_sever->nextPendingConnection();
        MultiSocket* t = new MultiSocket(tcp_socket,this);

        connect(t,SIGNAL(readyRead(MultiSocket*)),this,SLOT(multi_readyread(MultiSocket*)));
        v_multi_socket.append(t);
    }
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


            QList<QHostAddress> list =QNetworkInterface::allAddresses();

            foreach (QHostAddress address, list)
            {
               if(address.protocol() ==QAbstractSocket::IPv4Protocol)
               {
                    ui->listWidget_ip->addItem(address.toString());

               }
            }
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


void MainWindow::on_listWidget_ip_itemPressed(QListWidgetItem *item)
{
    qDebug() << item->text();
    if(codeShow != NULL) codeShow->close();
    codeShow = new QRcodeShow((char*)QString("http://" + item->text() +":" + QString("%1").arg(ui->spinBox_port->value())).toStdString().data());
    codeShow->move(mousePos);
    qDebug() << mousePos;
    codeShow->show();
}

void MainWindow::mouseMoveEvent(QMouseEvent *p)
{
    mousePos = p->pos();
    if(codeShow != NULL)
    {
        codeShow->close();
        codeShow=NULL;
    }
}
