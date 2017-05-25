#include "backgroundwidget.h"

#include <QPainter>

BackgroundWidget::BackgroundWidget(QWidget *parent) :
    QWidget(parent)
{

}

BackgroundWidget::~BackgroundWidget() {

}

void BackgroundWidget::init(int seat) {
    if (seat == 1) {
        image.load("://image/boucher-swan.jpg");
    } else if (seat == 2) {
        image.load("://image/goya.jpg");
    } else if (seat == 3) {
        image.load("://image/fragonard.jpg");
    } else if (seat == 4) {
        image.load("://image/boucher.jpg");
    } else if (seat == 5) {
        image.load("://image/rembrandt.jpg");
    } else {
        image.load("://misc/1px.png");
    }
    update();
}

void BackgroundWidget::init(QString file) {
    image.load(file);
    update();
}

void BackgroundWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
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
    } else {
        QWidget::paintEvent(event);
    }
}
