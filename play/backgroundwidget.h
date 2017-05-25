#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <QWidget>
#include <QPixmap>

class BackgroundWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BackgroundWidget(QWidget *parent = 0);
    ~BackgroundWidget();
    void init(int seat);
    void init(QString file);

signals:

public slots:

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QPixmap image;

};

#endif // BACKGROUNDWIDGET_H
