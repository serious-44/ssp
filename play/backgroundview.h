#ifndef BACKGROUNDVIEW_H
#define BACKGROUNDVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QPainter>
#include <QRectF>
#include <QPixmap>

class BackgroundView : public QGraphicsView
{
public:
    explicit BackgroundView(QWidget *parent = 0);
    ~BackgroundView();
    void init(int seat);

signals:

public slots:

protected:
    virtual void drawBackground(QPainter *painter, const QRectF &rect);

private:
    QPixmap image;
};

#endif // BACKGROUNDVIEW_H
