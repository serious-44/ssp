#ifndef PREVIEWPRODUCER_H
#define PREVIEWPRODUCER_H

#include <QWidget>
#include <QDir>
#include <QFileInfo>
#include <QVector>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMediaPlayer>
#include <QVideoProbe>
#include <QVideoFrame>

namespace Ui {
class PreviewProducer;
}

class PreviewJob {
public:
    QString ts;
    QString url;
    qint64 timestamp;
    int x, y, w, h;
};

class PreviewProducer : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewProducer(QWidget *parent = 0);
    ~PreviewProducer();
    void prepareClose();

public slots:
    void slotClose();
    void slotSelectDir();
    void slotRun();

    void startImage();
    void slotNetworkError(QNetworkReply::NetworkError err);
    void slotNetworkUpdateProgress(qint64 read, qint64 total);
    void slotNetworkFinished();
    void slotProcessVideo();
    void slotProcessVideoFrame(const QVideoFrame &buffer);
    void slotProcessNetImage();
    void slotAfterProcessImage(); //FIXME clear slotProcessVideoFrame queue

signals:

private:
    void process(QDir dir);
    void process(QFileInfo tsFile);
    void saveImage(QImage image);

    Ui::PreviewProducer *ui;

    QString lastDir;
    QVector<PreviewJob> todo;
    QNetworkAccessManager networkAccessManager;
    QNetworkReply *networkReply;

    QMediaPlayer *mediaPlayer;
    QVideoProbe *videoProbe;

};

#endif // PREVIEWPRODUCER_H
