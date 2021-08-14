#ifndef QRCODESHOW_H
#define QRCODESHOW_H

#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QImage>

#include "qrencode.h"
namespace Ui {
class QRcodeShow;
}

class QRcodeShow : public QWidget
{
    Q_OBJECT

public:
    explicit QRcodeShow(char* str,QWidget *parent = nullptr);
    ~QRcodeShow();

private:
    Ui::QRcodeShow *ui;
};

#endif // QRCODESHOW_H
