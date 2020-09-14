#include "webkitformboundary_parse.h"
#include <qdebug.h>
WebKitFormBoundary_parse::WebKitFormBoundary_parse(QString data)
{
    QStringList data_list = data.split("\r\n");
    int num = data_list[0].indexOf("WebKit");
    this->boundary = data_list[0].mid(num);

    for(int i = 0;i<data_list.size();i++){
        int num_1 = data_list[i].indexOf("\"");
        int num_2 = data_list[i].indexOf("\"",num_1+1);

        QString key = data_list[i].mid(num_1 + 1,num_2 - num_1 - 1);
        if(key == "chunk")
            this->chunk = data_list.at(i+2);
        else if(key == "chunks")
            this->chunks = data_list.at(i+2);
        else if(key == "name")
            this->name = data_list.at(i+2);
        else if(key == "uuid")
            this->uuid = data_list.at(i+2);
    }

    num = data.indexOf("Content-Type:");
    head_size = data.indexOf("\r\n",num) + 4;
    head_size = data.mid(0,head_size).toUtf8().length();

    foot_size = this->boundary.length() + 12;
}

WebKitFormBoundary_parse::WebKitFormBoundary_parse()
{

}
