#ifndef WEBKITFORMBOUNDARY_PARSE_H
#define WEBKITFORMBOUNDARY_PARSE_H

#include <QString>
#include <QStringList>
#include <QByteArray>
class WebKitFormBoundary_parse
{
public:
    WebKitFormBoundary_parse(QString data);
    WebKitFormBoundary_parse();

    QString chunk;
    QString chunks;
    QString name;
    QString uuid;
    QString boundary = "";

    int head_size = 0;
    int foot_size = 0;

};

#endif // WEBKITFORMBOUNDARY_PARSE_H
