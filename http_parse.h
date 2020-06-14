#ifndef HTTP_PARSE_H
#define HTTP_PARSE_H
#include <QString>
#include <QStringList>
#include <QDebug>
class http_parse
{
public:
    http_parse(QString http_data);
    QString type;
    QString path;
    QString version;
    QStringList http_head;
    QString http_content;
    QString origin_str;
    QString host = "";

    int Content_Length = -1;

};

#endif // HTTP_PARSE_H
