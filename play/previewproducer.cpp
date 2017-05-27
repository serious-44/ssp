#include "previewproducer.h"
#include "ui_previewproducer.h"

#include "util.h"

#include <QDir>
#include <QSettings>
#include <QFileDialog>
#include <QTextStream>
#include <QImageReader>
#include <QTimer>

PreviewProducer::PreviewProducer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreviewProducer),
    networkReply(0),
    mediaPlayer(0),
    videoProbe(0)
{
    ui->setupUi(this);

    connect(ui->buttonClose, SIGNAL (clicked()), this, SLOT (slotClose()));
    connect(ui->buttonSelectDir, SIGNAL (clicked()), this, SLOT (slotSelectDir()));
    connect(ui->buttonRun, SIGNAL (clicked()), this, SLOT (slotRun()));


    QString dir = QDir::homePath();
    QSettings settings;
    if (settings.contains("previewproducer/geometry")) {
        QRect geometry = settings.value("previewproducer/geometry").toRect();
        bool fullScreen = settings.value("previewproducer/fullScreen").toBool();
        setGeometry(geometry);
        if (fullScreen) {
            setWindowState(Qt::WindowFullScreen);
        }
        if (settings.contains("directory/video")) {
            dir = settings.value("directory/video").toString();
        }
    }
    ui->lineEditDir->setText(dir);
}

PreviewProducer::~PreviewProducer() {
    if (mediaPlayer) {
        mediaPlayer->stop();
        mediaPlayer->deleteLater();
        videoProbe->deleteLater();
    }
}

void PreviewProducer::prepareClose() {
    QSettings settings;
    settings.setValue("previewproducer/geometry", geometry());
    settings.setValue("previewproducer/fullScreen", isFullScreen());
}

void PreviewProducer::slotClose() {
    hide();
}

void PreviewProducer::slotSelectDir() {
    QFileDialog dialog(this, "Select the directory", ui->lineEditDir->text());
    dialog.setFileMode(QFileDialog::Directory);
    //dialog.setOption(QFileDialog::ShowDirsOnly, true);

    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
        QString dir = fileNames[0];
        ui->lineEditDir->setText(dir);

        QSettings settings;
        settings.setValue("directory/video", dir);
    }
}

void PreviewProducer::slotRun() {
    ui->buttonRun->setEnabled(false);
    ui->textLog->clear();
    ui->textLog->append("Prepare...");

    todo.clear();
    lastDir = "";
    QDir dir(ui->lineEditDir->text());
    process(dir);

    if (todo.isEmpty()) {
        ui->textLog->append("Nothing to do");
        ui->buttonRun->setEnabled(true);
    } else {
        ui->textLog->append("Generate...");
        startImage();
    }
}

void PreviewProducer::process(QDir dir) {
    ui->textLog->append(dir.absolutePath());
    dir.setSorting(QDir::DirsLast | QDir::Name);
    if (dir.exists()) {
        QStringList entries = dir.entryList();
        for (QString name : entries) {
            QFileInfo fi(dir, name);
            if (name.startsWith(".")) {
            } else if (fi.isDir() && ui->checkSubdirectories->isChecked()) {
                process(QDir(fi.absoluteFilePath()));
            } else if (name.endsWith(".ts")) {
                QString jpg = name;
                jpg.replace(Util::regPatternTs, ".jpg");
                if (!ui->checkReplace->isChecked() && entries.contains(jpg)) {
                    ui->textLog->append(QString ("<span style=\"color:#20c020;\"> %1 exists</span>").arg(jpg));
                } else {
                    process(fi);
                }
            }
        }
    } else {
        ui->textLog->append("<span style=\"color:#c02020;\">Can't read " + dir.absolutePath() + "</span>");
    }
}

void PreviewProducer::process(QFileInfo tsFile) {
    PreviewJob job;
    job.ts = tsFile.absoluteFilePath();
    job.error = false;
    int done = 0;

    QFile inFile(tsFile.absoluteFilePath());
    if (!inFile.open(QIODevice::ReadOnly)) {
        ui->textLog->append("<span style=\"color:#c02020;\">Can't read " + tsFile.absoluteFilePath() + "</span>");
        return;
    }
    QString buf;
    QTextStream in(&inFile);
    while (done != 3 && in.readLineInto(&buf)) {
        if (Util::regPreviewUrl.indexIn(buf) >= 0) {
            job.url = Util::regPreviewUrl.cap(1);
            done |= 1;
        } else if (Util::regPreviewTimestamp.indexIn(buf) >= 0) {
            job.timestamp = Util::timecodeToTimestamp(Util::regPreviewTimestamp);
            done |= 1;
        } else if (Util::regPreviewCrop.indexIn(buf) >= 0) {
            job.x = Util::regPreviewCrop.cap(1).toInt();
            job.y = Util::regPreviewCrop.cap(2).toInt();
            job.w = Util::regPreviewCrop.cap(3).toInt();
            job.h = Util::regPreviewCrop.cap(4).toInt();
            done |= 2;
        }
    }

    if (done != 3) {
        ui->textLog->append("<span style=\"color:#c02020;\">" + tsFile.fileName() + " no preview configured</span>");
    } else {
        //QString jpg = tsFile.baseName() + ".jpg";
        //ui->textLog->append("<span style=\"color:#2020c0;\">" + jpg + " created</span>");
        todo.append(job);
    }
}

void PreviewProducer::startImage() {
    //qDebug() << "process start next" << todo.length();
    if (todo.isEmpty()) {
        ui->textLog->append("Done");
        ui->buttonRun->setEnabled(true);
        return;
    }

    if (todo[0].url.isEmpty()) {
        QTimer::singleShot(0, this, SLOT(slotProcessVideo()));
    } else {
        QTimer::singleShot(0, this, SLOT(slotProcessNetImage()));
    }
}

void PreviewProducer::slotProcessNetImage() {
    ui->textLog->append(QString("Download %1").arg(todo[0].url));
    //qDebug() << "processNetImage" << todo[0].url;
    request.setUrl(todo[0].url);
    networkReply = networkAccessManager.get(request);
    connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotNetworkError(QNetworkReply::NetworkError)));
    connect(networkReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(slotNetworkUpdateProgress(qint64, qint64)));
    connect(networkReply, SIGNAL(finished()), this, SLOT(slotNetworkFinished()));
}

void PreviewProducer::slotNetworkError(QNetworkReply::NetworkError err) {
    //qDebug() << "processNetImage ERROR" << todo[0].url;
    if (!todo[0].error) {
        todo[0].error = true;
        ui->textLog->append(QString ("<span style=\"color:#c02020;\">Error %1 fetching %2</span>").arg(networkReply->errorString()).arg(todo[0].url));
        QTimer::singleShot(500, this, SLOT(slotAfterProcessImage()));
    }
}

void PreviewProducer::slotNetworkUpdateProgress(qint64 read, qint64 total) {
    //qDebug() << "processNetImage ..." << todo[0].url;
}

void PreviewProducer::slotNetworkFinished() {
    //qDebug() << "processNetImage Download" << todo[0].url;
    QImage p;
    QByteArray b = networkReply->read(networkReply->size());
    p.loadFromData(b);
    networkReply->deleteLater();
    networkReply = 0;
    saveImage(p);
}


void PreviewProducer::slotProcessVideo() {
    //qDebug() << "processVideo";
    if (!mediaPlayer) {
        mediaPlayer = new QMediaPlayer();
        connect(mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(slotMediaPlayerError(QMediaPlayer::Error)));
        videoProbe = new QVideoProbe();
        connect(videoProbe, SIGNAL(videoFrameProbed(const QVideoFrame &)), this, SLOT(slotProcessVideoFrame(const QVideoFrame &)));
        videoProbe->setSource(mediaPlayer);
    } else {
        mediaPlayer->stop();
    }
    QString avi(todo[0].ts);
    avi.replace(Util::regPatternTs, ".avi");
    ui->textLog->append(QString("Open %1").arg(avi));
    mediaPlayer->setMedia(QUrl::fromLocalFile(avi));
    mediaPlayer->play();
    mediaPlayer->pause();
    qint64 start = todo[0].timestamp - 100;
    if (start < 0) {
        start = 0;
    }
    mediaPlayer->setPosition(start);
    mediaPlayer->play();
}

void PreviewProducer::slotMediaPlayerError(QMediaPlayer::Error err) {
    //qDebug() << "mediaplayer ERROR" << err << todo[0].url;
    if (!todo[0].error) {
        todo[0].error = true;
        ui->textLog->append(QString ("<span style=\"color:#c02020;\">Error in Mediaplayer: %1</span>").arg(mediaPlayer->errorString()));
        mediaPlayer->stop();
        QTimer::singleShot(500, this, SLOT(slotAfterProcessImage()));
    }
}

void PreviewProducer::slotProcessVideoFrame(const QVideoFrame &buffer) {
    if (todo.isEmpty()) {
        return;
    }
    if (buffer.startTime() >= todo[0].timestamp) {
        //qDebug() << "processVideo Found";
        mediaPlayer->stop();

        QImage img;
        QVideoFrame frame(buffer);
        frame.map(QAbstractVideoBuffer::ReadOnly);
        //qDebug() << frame.pixelFormat();
        QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat());
        // BUT the frame.pixelFormat() is QVideoFrame::Format_Jpeg, and this is
        // mapped to QImage::Format_Invalid by
        // QVideoFrame::imageFormatFromPixelFormat
        if (imageFormat != QImage::Format_Invalid) {
            img = QImage(frame.bits(), frame.width(), frame.height(), imageFormat);
        } else if (frame.pixelFormat() == QVideoFrame::Format_YUV420P) {
            int width = frame.width();
            int height = frame.height();
            img = QImage(width, height, QImage::Format_RGB32);
            uchar *b = frame.bits();
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int y_ = b[y * width + x];
                    int u_ = b[(width*height) + ((y/2) *(width/2)) + x/2];
                    int v_ = b[((width*height) + ((width*height)/4) + ((y/2) *(width/2)) + x/2)];

                    int c_ = y_ - 16;
                    int d_ = u_ - 128;
                    int e_ = v_ - 128;
                    int r = (298 * c_ + 409 * e_ + 128) >> 8;
                    int g = (298 * c_ - 100 * d_ - 208 * e_ + 128) >> 8;
                    int b = (298 * c_ + 516 * d_ + 128) >> 8;

                    if (b < 0) b = 0;
                    if (b > 255) b = 255;
                    if (r < 0) r = 0;
                    if (r > 255) r = 255;
                    if (g < 0) g = 0;
                    if (g > 255) g = 255;

                    img.setPixel(x, y, qRgb(int(r), int(g), int(b)));
                }
            }
        } else {
            // e.g. JPEG
            int nbytes = frame.mappedBytes();
            img = QImage::fromData(frame.bits(), nbytes);
        }
        frame.unmap();

        todo[0].timestamp = LLONG_MAX;
        saveImage(img);
    }
}

void PreviewProducer::saveImage(QImage image) {
    //qDebug() << "save image" << todo[0].ts;
    if (image.isNull()) {
        ui->textLog->append(QString ("<span style=\"color:#c02020;\">Unreconized image %1</span>").arg(todo[0].url));
    } else {
        QImage crop = image.copy(todo[0].x, todo[0].y, todo[0].w, todo[0].h);

        QString jpg(todo[0].ts);
        jpg.replace(Util::regPatternTs, ".jpg");

        QString backFileName(jpg);
        backFileName += ".bak";
        QFile backFile(backFileName);
        if (backFile.exists()) {
            if (!backFile.remove()) {
                ui->textLog->append(QString ("<span style=\"color:#c02020;\">Can't remove %1</span>").arg(backFileName));
            }
        }
        QFile oldFile(jpg);
        if (oldFile.exists()) {
            if (!oldFile.rename(backFileName)) {
                ui->textLog->append(QString ("<span style=\"color:#c02020;\">Can't rename %1</span>").arg(backFileName));
            }
        }
        QFile outFile(jpg);
        if (!outFile.open(QIODevice::WriteOnly)) {
            ui->textLog->append(QString ("<span style=\"color:#c02020;\">Can't write %1</span>").arg(jpg));
            return;
        }
        crop.save(&outFile, "JPG");
        QFileInfo f(jpg);
        QString dir(f.absolutePath());
        if (dir != lastDir) {
            lastDir = dir;
            ui->textLog->append(dir);
        }
        ui->textLog->append("<span style=\"color:#2020c0;\">" + f.fileName() + " created</span>");
    }

    QTimer::singleShot(200, this, SLOT(slotAfterProcessImage()));
}
void PreviewProducer::slotAfterProcessImage() {
    //qDebug() << "after save image";
    todo.remove(0);
    startImage();
}

