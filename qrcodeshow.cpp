#include "qrcodeshow.h"
#include "ui_qrcodeshow.h"
#include "math.h"
#include <QDebug>
QRcodeShow::QRcodeShow(char* str,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QRcodeShow)
{
    ui->setupUi(this);
    QRcode *pQRC = QRcode_encodeString(str, 0, QR_ECLEVEL_H, QR_MODE_8, 1 );
    QImage img(pQRC->width,pQRC->width,QImage::Format_Grayscale8);

    int height = 0;
    for(int i=0;i<pQRC->width * pQRC->width;i++)
    {
        int  color = 255 - (pQRC->data[i]&1)*255;
        img.setPixel(height,i%pQRC->width,qRgb(color,color,color));//(pQRC->data[i]&1)*255);
        if(i%pQRC->width == 0 && i!=0)
            height++;
    }

    img = img.scaled(QSize(300,300));
    ui->label->setPixmap(QPixmap::fromImage(img));
    this->setWindowFlag(Qt::FramelessWindowHint);
}

QRcodeShow::~QRcodeShow()
{
    delete ui;
}
