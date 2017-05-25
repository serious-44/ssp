#include "playerbox.h"

#include "util.h"

#include  <QDebug>
#include  <QPainter>
#include  <QFile>
#include  <QFileInfo>
#include  <QImage>

PlayerBox::PlayerBox(QWidget *parent) :
    QWidget(parent),
    active(false)
{
    image.load("://misc/empty_group.png");
}

void PlayerBox::setTsFileName(QString tsFileName) {
    jpgFileName = tsFileName;
    if (jpgFileName.isEmpty()) {
        image.load("://misc/empty_group.png");
    } else {
        jpgFileName.replace(Util::regPatternTs, ".jpg");
        QFileInfo jpgFile(jpgFileName);
        if (jpgFile.exists()) {
            loadImage();
        } else {
            image.load("://misc/no_preview.png");
        }
    }
    update();
}

void PlayerBox::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::Antialiasing);

    if(!image.isNull()) {
        float imageRatio = (float)image.width() / image.height();
        float widgetRatio = (float)width() / height();
        if(imageRatio > widgetRatio) {
            int newWidth = (int)(height() * imageRatio);
            int offset = (newWidth - width()) / -2;
            painter.drawPixmap(offset, 0, newWidth, height(), image);
        } else {
            int newHeight = (int)(width() / imageRatio);
            int offset = (newHeight - height()) / -2;
            painter.drawPixmap(0, offset, width(), newHeight, image);
        }
    }
    QColor erase(24, 4, 56);
    QColor c(24, 4, 56, 110);
    //QColor ca1(200, 200, 0, 130);
    QColor ca2(200, 200, 0);

    QPen np(Qt::NoPen);
    painter.setPen(np);
    QBrush b(Qt::SolidPattern);
    b.setColor(erase);
    painter.setBrush(b);
    QPoint points1[] = { QPoint(0, 0), QPoint(5, 0), QPoint(0, 5) };
    QPoint points2[] = { QPoint(width(), 0), QPoint(width()-5, 0), QPoint(width(), 5) };
    QPoint points3[] = { QPoint(0, height()), QPoint(5, height()), QPoint(0, height()-5) };
    QPoint points4[] = { QPoint(width(), height()), QPoint(width()-6, height()), QPoint(width(), height()-6) };
    painter.drawPolygon(points1, 3);
    painter.drawPolygon(points2, 3);
    painter.drawPolygon(points3, 3);
    painter.drawPolygon(points4, 3);

    QBrush nb(Qt::NoBrush);
    painter.setBrush(nb);

    QPen p;
    p.setColor(c);
    p.setWidth(11);
    painter.setPen(p);
    painter.drawRoundedRect(0, 0, width() - 1, height() - 1, 10, 10);
    p.setWidth(9);
    painter.setPen(p);
    painter.drawRoundedRect(0, 0, width() - 1, height() - 1, 10, 10);
    p.setWidth(7);
    painter.setPen(p);
    painter.drawRoundedRect(0, 0, width() - 1, height() - 1, 10, 10);
    p.setWidth(5);
    painter.setPen(p);
    painter.drawRoundedRect(0, 0, width() - 1, height() - 1, 10, 10);
    if (active) {
        p.setWidth(3);
        p.setColor(ca2);
        painter.setPen(p);
        painter.drawRoundedRect(3, 3, width() - 1 - 6, height() - 1 - 6, 10, 10);
    } else {
        p.setWidth(3);
        painter.setPen(p);
        painter.drawRoundedRect(0, 0, width() - 1, height() - 1, 10, 10);
        p.setWidth(1);
        painter.setPen(p);
        painter.drawRoundedRect(0, 0, width() - 1, height() - 1, 10, 10);
    }

    QWidget::paintEvent(event);
}

void PlayerBox::setActive(bool  a) {
    if (a != active) {
        active = a;
        update();
    }
}

void PlayerBox::loadImage() {
    QImage orgImg(jpgFileName);

    QImage img(orgImg.width(), orgImg.height(), QImage::Format_ARGB32);

    for (int y = 0; y < orgImg.height(); y++) {
        for (int x = 0; x < orgImg.width(); x++ ) {
            QColor org = orgImg.pixel(x, y);
            //QColor n(org.red(), org.green(), org.blue(), 150);
            int h, s, v;
            org.getHsv(&h, &s, &v);
            QColor n = QColor::fromHsv(h, s*2/3, v/2, 150);
            img.setPixel(x, y, n.rgba());
        }
    }
    image.convertFromImage(img);
}
