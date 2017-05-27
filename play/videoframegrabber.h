#ifndef VIDEOFRAMEGRABBER_H
#define VIDEOFRAMEGRABBER_H

//https://stackoverflow.com/questions/37724602/how-to-save-a-frame-using-qmediaplayer

#include <QtWidgets>
#include <QAbstractVideoSurface>

class VideoFrameGrabber : public QAbstractVideoSurface
{
    Q_OBJECT

public:
    VideoFrameGrabber(QObject *parent = 0);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
    bool isFormatSupported(const QVideoSurfaceFormat &format) const;

    bool start(const QVideoSurfaceFormat &format);
    void stop();

    bool present(const QVideoFrame &frame);

private:
    QImage::Format imageFormat;
    QRect targetRect;
    QSize imageSize;
    QRect sourceRect;
    QVideoFrame currentFrame;

signals:
    void frameAvailable(const QVideoFrame &buffer);
};

#endif // VIDEOFRAMEGRABBER_H
