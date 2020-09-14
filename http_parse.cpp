#include "http_parse.h"

http_parse::http_parse(QByteArray data)
{
    QString http_data = data;
    this->origin_str = http_data;

    QStringList http_data_list = http_data.split("\r\n\r\n");
    QString http_data_head = http_data_list[0];
    this->http_content = data.mid(http_data_head.toUtf8().length() + 4);

    this->http_head = http_data_head.split("\r\n");
    QStringList http_data_head_line1_list = http_head[0].split(" ");

    this->type = http_data_head_line1_list[0];
    this->path = http_data_head_line1_list[1];
    this->version = http_data_head_line1_list[2];

    foreach (QString line, http_head)
    {
        if(this->Content_Length == -1)
        {
            int num = line.indexOf("Content-Length:");
            if(num != -1)
            {
                this->Content_Length = line.mid(num +16).toInt();
            }
        }
        if(this->host == "")
        {
            int num = line.indexOf("Host: ");
            if(num != -1)
            {
                this->host = line.mid(num + 6);
            }
        }

    }
}
