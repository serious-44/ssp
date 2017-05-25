#include "cashimage.h"

#include  <QDebug>
#include  <QPainter>
/*

39 30000
38 25000 -10
37 21000 -9
36 17000 -8
35 14500 -5
34 12000 -5
33 10000 -4
32 8500 -3
31 7000 -3
30 6000 -2
29 5000 -2
28 4000 -2
27 3500 -1
26 3000 -1
25 2500 -1
24 2000 -1
23 1500 -1
22 1000 -1
21 940 -13
20 780 -12
19 650 -11
18 540 -10
17 450 -9
16 380 -7
15 310 -7
14 260 -5
13 210 -5
12 180 -3
11 150 -3
10 120 -3
 9 110 -1
 8 100 -1
 7 90 -1
 6 80 -1
 5 70 -1
 4 60 -1
 3 50 -1
 2 40 -1
 1 30 -1

*/
CashImage::CashImage(QWidget *parent) :
    QWidget(parent),
    imagePos(-1)
{
    for (int i = 0; i < 39; i++) {
        image[i].load(QString::asprintf("://coins/%04d.png", i+1));
    }
}

int CashImage::amountOfFile[] { 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 150, 180, 210, 260, 310, 380, 450, 540, 650, 780, 940, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 5000, 6000, 7000, 8500, 10000, 12000, 14500, 17000, 21000, 25000, 0xffffff};

void CashImage::setAmount(int a) {
    if (a == 0) {
        imagePos = -1;
    } else {
        imagePos = 0;
        while (a > amountOfFile[imagePos]) {
            imagePos++;
        }
    }
    update();
}

void CashImage::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::Antialiasing);

    if(imagePos >= 0) {
        float imageRatio = (float)image[imagePos].width() / image[imagePos].height();
        float widgetRatio = (float)width() / height();
        if(imageRatio < widgetRatio) {
            int newWidth = (int)(height() * imageRatio);
            int offset = (newWidth - width()) / -2;
            painter.drawPixmap(offset, 0, newWidth, height(), image[imagePos]);
        } else {
            int newHeight = (int)(width() / imageRatio);
            int offset = (newHeight - height()) / -2;
            painter.drawPixmap(0, offset, width(), newHeight, image[imagePos]);
        }
    }
}
