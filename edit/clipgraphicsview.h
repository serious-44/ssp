#ifndef CLIPGRAPHICSVIEW_H
#define CLIPGRAPHICSVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QGraphicsPixmapItem>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QMouseEvent>

class ClipGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ClipGraphicsView(QWidget *parent = 0);
    ~ClipGraphicsView();
    void init(QGraphicsScene *s, QGraphicsVideoItem *i, QGraphicsPixmapItem *p);
    void videoChanged();
    void imageChanged();
    void showItem(int num);
    QImage trimedPreview();
    QRect trimedRect();

public slots:
    void slotTrimPreview(bool on);

private slots:
    void slotRubberBandChanged(QRect rubberBandRect, QPointF fromScenePoint, QPointF toScenePoint);

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

private:

    QGraphicsScene *scene;
    QGraphicsVideoItem *item1;
    QGraphicsPixmapItem *item2;

    enum DragArea { DragNone, DragTopLeft, DragTopRight, DragBottomLeft, DragBottomRight, DragAll};

    bool trimPreview;
    int selectionForWidth;
    int selectionForHeight;
    int selectionX;
    int selectionY;
    int selectionW;
    int selectionH;
    enum DragArea dragArea;
    int dragStartX;
    int dragStartY;
    int startSelectionX1;
    int startSelectionY1;
    int startSelectionX2;
    int startSelectionY2;

};

#endif // CLIPGRAPHICSVIEW_H
