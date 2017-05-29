#include "backgroundview.h"


BackgroundView::BackgroundView(QWidget *parent) :
    QGraphicsView(parent)
{

}

BackgroundView::~BackgroundView() {

}

void BackgroundView::init(int seat) {
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


void BackgroundView::drawBackground(QPainter *painter, const QRectF &rect) {
    painter->setRenderHint(QPainter::Antialiasing);

    if(!image.isNull()) {
        QTransform org = painter->transform();
        painter->resetTransform();
        float imageRatio = (float)image.width() / image.height();
        float widgetRatio = (float)width() / height();
        if(imageRatio > widgetRatio) {
            int newWidth = (int)(height() * imageRatio);
            int offset = (newWidth - width()) / -2;
            painter->drawPixmap(offset, 0, newWidth, height(), image);
        } else {
            int newHeight = (int)(width() / imageRatio);
            int offset = (newHeight - height()) / -2;
            painter->drawPixmap(0, offset, width(), newHeight, image);
        }
        painter->setTransform(org);
    }
}
