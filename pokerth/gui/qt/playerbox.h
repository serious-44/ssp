#ifndef PLAYERBOX_H
#define PLAYERBOX_H

#include <QWidget>
#include <QFrame>

class PlayerBox : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerBox(QWidget *parent = 0);
    void setTsFileName(QString tsFileName);
    void setActive(bool);

protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:

private:
    QString jpgFileName;
    QPixmap image;
    bool active;

    void loadImage();
};

#endif // PLAYERBOX_H
