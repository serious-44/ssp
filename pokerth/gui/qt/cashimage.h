#ifndef CASHIMAGE_H
#define CASHIMAGE_H

#include <QWidget>

class CashImage : public QWidget
{
    Q_OBJECT
public:
    explicit CashImage(QWidget *parent = 0);
    void setAmount(int a);

protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:

private:
    static int amountOfFile[39];
    QPixmap image[39];
    int imagePos;
};

#endif // CASHIMAGE_H
