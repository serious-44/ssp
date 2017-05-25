#include "clipgraphicsview.h"

#include <QDebug>
#include <QSizeF>
#include <QRectF>

ClipGraphicsView::ClipGraphicsView(QWidget *parent) :
    QGraphicsView(parent),
    scene(0),
    item1(0),
    item2(0),
    selectionForWidth(-1),
    selectionForHeight(-1)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setDragMode(QGraphicsView::ScrollHandDrag);
    //setViewportMargins(20, 20, 0, 0);

    //setViewportUpdateMode(QGraphicsView::NoViewportUpdate);

    trimPreview = false;

    connect(this, SIGNAL(rubberBandChanged(QRect, QPointF, QPointF)), this, SLOT(slotRubberBandChanged(QRect, QPointF, QPointF)));
}

ClipGraphicsView::~ClipGraphicsView() {

}

void ClipGraphicsView::init(QGraphicsScene *s, QGraphicsVideoItem *i, QGraphicsPixmapItem *p) {
    scene = s;
    item1 = i;
    item2 = p;
    item2->setVisible(false);
    setScene(scene);
    scene->addItem(item1);
    scene->addItem(item2);
}

void ClipGraphicsView::showItem(int num) {
    if (num == 0) {
        item1->show();
        item2->hide();
        videoChanged();
    } else {
        item1->hide();
        item2->show();
        imageChanged();
    }
}

void ClipGraphicsView::videoChanged() {
    if (item1->isVisible()) {
        item1->setSize(item1->nativeSize());
        fitInView(item1, Qt::KeepAspectRatio);
    }
    //qDebug() << "videoChanged" << height() << width() << "/" << item->nativeSize().width() << item->nativeSize().height();
}

void ClipGraphicsView::imageChanged() {
    if (item2->isVisible()) {
        fitInView(item2, Qt::KeepAspectRatio);
    }
    //qDebug() << "videoChanged" << height() << width() << "/" << item->nativeSize().width() << item->nativeSize().height();
}

void ClipGraphicsView::slotTrimPreview(bool on) {
    //qDebug() << "trimPreview" << on;
    trimPreview = on;
    if (on) {
        //setDragMode(QGraphicsView::ScrollHandDrag);
        //setDragMode(QGraphicsView::RubberBandDrag);
        setCursor(Qt::CrossCursor);
        if (selectionForHeight != height() || selectionForWidth != width()) {
            int h1 = height() * 3 / 4;
            int w1 = h1 * 4 / 3;
            int w2 = width() * 3 / 4;
            if (w1 < w2) {
                selectionW = w1;
                selectionH = h1;
            } else {
                selectionW = w2;
                selectionH = w2 * 3 / 4;
            }
            selectionX = (width() - selectionW) / 2;
            selectionY = (height() - selectionH) / 2;
            selectionForHeight = height();
            selectionForWidth = width();
        }
    } else {
        setCursor(Qt::ArrowCursor);
        //setDragMode(QGraphicsView::NoDrag);
    }
    dragArea = DragNone;
    viewport()->update();
    //repaint();
}

QImage ClipGraphicsView::trimedPreview() {
    //QSizeF videoSize = item->nativeSize();
    QRectF sceneRect = scene->sceneRect();

    QRectF target;
    QRectF source;
    if (trimPreview) {
        QTransform v = viewportTransform();
        qreal scale = v.m11();
        int tx = v.dx();
        int ty = v.dy();
        qreal selX = (selectionX - tx) / scale;
        qreal selY = (selectionY - ty) / scale;
        qreal selW = selectionW / scale;
        qreal selH = selectionH / scale;
        source = QRectF(selX, selY, selW, selH);
        //target = QRectF(0, 0, 500, selectionH * 500 / selectionW);
        target = QRectF(0, 0, selW, selH);
    } else {
        //FIXME branch not used
        source = sceneRect;
        target = sceneRect;
        target.moveTo(0, 0);
        target.setHeight(target.height() * 500 / target.width());
        target.setWidth(500);
    }

    //qDebug() << width() << height() << sceneRect << target << (target.width()/target.height()) << source << (source.width()/source.height());

    QImage img(target.width(), target.height(), QImage::Format_RGB32);
    QPainter painter(&img);
    scene->render(&painter, target, source);
    return img;
}

QRect ClipGraphicsView::trimedRect() {
    if (trimPreview) {
        QTransform v = viewportTransform();
        qreal scale = v.m11();
        int tx = v.dx();
        int ty = v.dy();
        qreal selX = (selectionX - tx) / scale;
        qreal selY = (selectionY - ty) / scale;
        qreal selW = selectionW / scale;
        qreal selH = selectionH / scale;

        QRectF sceneRect = scene->sceneRect();
        if (selX > sceneRect.width() - 10) {
            selX = sceneRect.width() - 10;
        }
        if (selY > sceneRect.height() - 10) {
            selY = sceneRect.height() - 10;
        }
        if (selX < 0) {
            selW += selX;
            selX = 0;
        }
        if (selY < 0) {
            selH += selY;
            selY = 0;
        }
        if (selX + selW > sceneRect.width()) {
            selW = sceneRect.width() - selX;
        }
        if (selY + selH > sceneRect.height()) {
            selH = sceneRect.height() - selY;
        }
        return QRect((int)selX, (int)selY, (int)selW, (int)selH);
    } else {
        //FIXME branch not used
        return QRect((int)(scene->sceneRect().x()), (int)(scene->sceneRect().x()), (int)(scene->sceneRect().width()), (int)(scene->sceneRect().height()));
    }
}

void ClipGraphicsView::slotRubberBandChanged(QRect rubberBandRect, QPointF fromScenePoint, QPointF toScenePoint) {
    //qDebug() << "rubber" << rubberBandRect << fromScenePoint << toScenePoint;
}

void ClipGraphicsView::mouseMoveEvent(QMouseEvent *event) {
    //qDebug() << "mouseMove";
    bool done = false;
    int x1, y1, x2, y2;
    if (dragArea == DragTopLeft) {
        x1 = startSelectionX1 + (event->x() - dragStartX);
        y1 = startSelectionY1 + (event->y() - dragStartY);
        x2 = startSelectionX2;
        y2 = startSelectionY2;
        if (x1 > x2 - 20) x1 = x2 - 20;
        if (x1 < 0) x1 = 0;
        if (y1 > y2 - 20) y1 = y2 - 20;
        if (y1 < 0) y1 = 0;
        done = true;
    } else if (dragArea == DragBottomLeft) {
        x1 = startSelectionX1 + (event->x() - dragStartX);
        y1 = startSelectionY1;
        x2 = startSelectionX2;
        y2 = startSelectionY2 + (event->y() - dragStartY);
        if (x1 > x2 - 20) x1 = x2 - 20;
        if (x1 < 0) x1 = 0;
        if (y1 > y2 - 20) y2 = y1 + 20;
        if (y2 > height() - 1) y2 = height() - 1;
        done = true;
    } else if (dragArea == DragTopRight) {
        x1 = startSelectionX1;
        y1 = startSelectionY1 + (event->y() - dragStartY);
        x2 = startSelectionX2 + (event->x() - dragStartX);
        y2 = startSelectionY2;
        if (x1 > x2 - 20) x2 = x1 + 20;
        if (x2 > width() - 1) x2 = width() - 1;
        if (y1 > y2 - 20) y1 = y2 - 20;
        if (y1 < 0) y1 = 0;
        done = true;
    } else if (dragArea == DragBottomRight) {
        x1 = startSelectionX1;
        y1 = startSelectionY1;
        x2 = startSelectionX2 + (event->x() - dragStartX);
        y2 = startSelectionY2 + (event->y() - dragStartY);
        if (x1 > x2 - 20) x2 = x1 + 20;
        if (x2 > width() - 1) x2 = width() - 1;
        if (y1 > y2 - 20) y2 = y1 + 20;
        if (y2 > height() - 1) y2 = height() - 1;
        done = true;
    } else if (dragArea == DragAll) {
        x1 = startSelectionX1 + (event->x() - dragStartX);
        y1 = startSelectionY1 + (event->y() - dragStartY);
        x2 = startSelectionX2 + (event->x() - dragStartX);
        y2 = startSelectionY2 + (event->y() - dragStartY);
        if (x1 < 0) {
            x2 -= x1;
            x1 = 0;
        }
        if (x2 > width() - 1) {
            x1 -= x2 - (width() - 1);
            x2 = width() - 1;
        }
        if (y1 < 0) {
            y2 -= y1;
            y1 = 0;
        }
        if (y2 > height() - 1) {
            y1 -= y2 - (height() - 1);
            y2 = height() - 1;
        }
        done = true;
    }

    if (done) {
        selectionX = x1;
        selectionY = y1;
        selectionW = x2 - x1;
        selectionH = y2 - y1;
        viewport()->update();
    }

}

void ClipGraphicsView::mousePressEvent(QMouseEvent *event) {
    //qDebug() << "mousePress" << event->x() << event->y() << "/" << selectionX << selectionY;
    if (event->button() == Qt::LeftButton) {
        dragArea = DragNone;
        dragStartX = event->x();
        dragStartY = event->y();
        startSelectionX1 = selectionX;
        startSelectionY1 = selectionY;
        startSelectionX2 = selectionX + selectionW;
        startSelectionY2 = selectionY + selectionH;
        if (dragStartX > selectionX - 10 && dragStartX < selectionX + 10 && dragStartY > selectionY - 10 && dragStartY < selectionY + 10) {
            //qDebug() << "TopLeft";
            dragArea = DragTopLeft;
        } else if (dragStartX > selectionX - 10 && dragStartX < selectionX + 10 && dragStartY > selectionY + selectionH - 10 && dragStartY < selectionY + selectionH + 10) {
            //qDebug() << "BottomLeft";
            dragArea = DragBottomLeft;
        } else if (dragStartX > selectionX + selectionW - 10 && dragStartX < selectionX + selectionW + 10 && dragStartY > selectionY - 10 && dragStartY < selectionY + 10) {
            //qDebug() << "TopRight";
            dragArea = DragTopRight;
        } else if (dragStartX > selectionX + selectionW - 10 && dragStartX < selectionX + selectionW + 10 && dragStartY > selectionY + selectionH - 10 && dragStartY < selectionY + selectionH + 10) {
            //qDebug() << "BottomRight";
            dragArea = DragBottomRight;
        } else if (dragStartX > selectionX && dragStartX < selectionX + selectionW && dragStartY > selectionY && dragStartY < selectionY + selectionH) {
            //qDebug() << "All";
            dragArea = DragAll;
        }
        event->accept();
    }

}

void ClipGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    //qDebug() << "mouseRelease";
    if (event->button() == Qt::LeftButton) {
        dragArea = DragNone;
    }
    event->accept();
}

void ClipGraphicsView::resizeEvent(QResizeEvent *event) {
    //qDebug() << "resize";
    fitInView(item1, Qt::KeepAspectRatio);
    fitInView(item2, Qt::KeepAspectRatio);
    QGraphicsView::resizeEvent(event);
    //qDebug() << "resize" << height() << width() << "/" << item->nativeSize().width() << item->nativeSize().height();
}

void ClipGraphicsView::paintEvent(QPaintEvent *event) {
    //qDebug() << "paint";
    QGraphicsView::paintEvent(event);

    if (trimPreview) {
        QPainter painter(viewport());
        QColor lg = QColor(100, 100, 100, 200);
        //painter.setBrush(lg);
        painter.fillRect(0, 0, selectionX, height(), lg);
        painter.fillRect(selectionX + selectionW, 0, width() - (selectionX + selectionW), height(), lg);
        painter.fillRect(selectionX, 0, selectionW, selectionY, lg);
        painter.fillRect(selectionX, selectionY + selectionH, selectionW, height() - (selectionY + selectionH), lg);
        painter.setPen(Qt::black);
        painter.drawRect(selectionX, selectionY, selectionW, selectionH);
        painter.drawRect(selectionX - 5, selectionY - 5, 10, 10);
        painter.drawRect(selectionX + selectionW - 5, selectionY - 5, 10, 10);
        painter.drawRect(selectionX - 5, selectionY + selectionH - 5, 10, 10);
        painter.drawRect(selectionX + selectionW - 5, selectionY + selectionH - 5, 10, 10);
    }
}
