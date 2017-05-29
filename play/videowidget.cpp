#include "videowidget.h"

#include "util.h"

#include <QDebug>
#include <QPainter>
#include <QVBoxLayout>

VideoWidget::VideoWidget(QWidget *parent) :
    QWidget(parent),
    seat(-1),
    active(false),
    playing(false),
    playingLoud(false),
    idle(false),
    needsStart(false),
    gameOver(true),
    videoWidth(-1),
    videoHeight(-1)
{
    //setAspectRatioMode(Qt::KeepAspectRatioByExpanding); klappt nicht

    for (int t = 0; t < 7; t++) {
        for (int i = 0; i < 5; i++) {
            videoClips[t][i] = QMap<QString, QVector<ClipTimes> >();
            videoClips[t][i]["intro"] = QVector<ClipTimes>();
            videoClips[t][i]["take"] = QVector<ClipTimes>();
            videoClips[t][i]["take_good"] = QVector<ClipTimes>();
            videoClips[t][i]["take_bad"] = QVector<ClipTimes>();
            videoClips[t][i]["drop"] = QVector<ClipTimes>();
            videoClips[t][i]["drop_throw"] = QVector<ClipTimes>();
            videoClips[t][i]["win"] = QVector<ClipTimes>();
            videoClips[t][i]["win_youlose"] = QVector<ClipTimes>();
            videoClips[t][i]["lose"] = QVector<ClipTimes>();
            videoClips[t][i]["lose_youwin"] = QVector<ClipTimes>();
            videoClips[t][i]["show"] = QVector<ClipTimes>();
            videoClips[t][i]["show_cards"] = QVector<ClipTimes>();
            videoClips[t][i]["show_high"] = QVector<ClipTimes>();
            videoClips[t][i]["show_low"] = QVector<ClipTimes>();
            videoClips[t][i]["show_drink"] = QVector<ClipTimes>();
            videoClips[t][i]["on"] = QVector<ClipTimes>();
            videoClips[t][i]["off"] = QVector<ClipTimes>();
            videoClips[t][i]["broke"] = QVector<ClipTimes>();
            videoClips[t][i]["no"] = QVector<ClipTimes>();
            videoClips[t][i]["no_cards"] = QVector<ClipTimes>();
            videoClips[t][i]["none"] = QVector<ClipTimes>();
        }
    }

    tryZoom = ClipType::ZOOM_OUT;
    tryLoud[0] = ClipType::LOUD;
    tryLoud[1] = ClipType::LOUD_ZOOM;
    tryQuiet[0] = ClipType::QUIET;
    tryQuiet[1] = ClipType::QUIET_ZOOM;

    idleTimer = new QTimer(this);
    idleTimer->setSingleShot(true);
    connect(idleTimer, SIGNAL(timeout()), this, SLOT(slotTimerElapsed()));

    stopTimer = new QTimer(this);
    stopTimer->setSingleShot(true);
    connect(stopTimer, SIGNAL(timeout()), this, SLOT(slotStopVideo()));

    doneTimer = new QTimer(this);
    doneTimer->setSingleShot(true);
    connect(doneTimer, SIGNAL(timeout()), this, SLOT(checkVideoQueue()));

    sequenceTimer = new QTimer(this);
    sequenceTimer->setSingleShot(true);
    sequenceTimer->setTimerType(Qt::PreciseTimer);
    connect(sequenceTimer, SIGNAL(timeout()), this, SLOT(checkVideoQueue()));

    mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setNotifyInterval(1000 / 25);
    connect(mediaPlayer, SIGNAL(metaDataChanged(const QString&, const QVariant&)), this, SLOT(slotMetaDataChanged(const QString &, const QVariant &)));
    connect(mediaPlayer, SIGNAL(metaDataAvailableChanged(bool)), this, SLOT(slotMetaDataAvailableChanged(bool)));
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(slotPositionChanged(qint64)));
    connect(mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(slotMediaPlayerError(QMediaPlayer::Error)));

#ifdef USE_VideoWidget
    backgroundWidget = new BackgroundWidget(this);
    videoWidget = new QVideoWidget(this);
    videoWidget->setVisible(false);
    videoWidget->setAspectRatioMode(Qt::IgnoreAspectRatio);
    mediaPlayer->setVideoOutput(videoWidget);
#endif
#ifdef USE_GraphicsScene
    view = new QGraphicsView(this);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scene = new QGraphicsScene(view);
    background = new QGraphicsPixmapItem();
    scene->addItem(background);
    item = new QGraphicsVideoItem();
    mediaPlayer->setVideoOutput(item);
    view->setScene(scene);
    scene->addItem(item);
    item->hide();
#endif
}

VideoWidget::~VideoWidget() {
    mediaPlayer->stop();
    mediaPlayer->deleteLater();
    mediaPlayer = 0;
    //videoWidget->deleteLater();
    //videoWidget = 0;
    //delete backgroundWidget;
}

void VideoWidget::init(int s) {
    seat = s;

#ifdef USE_VideoWidget
    backgroundWidget->init(s);
#endif
#ifdef USE_GraphicsScene
    QPixmap image;
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
    background->setPixmap(image);
    resizeBackground();
#endif
}

void VideoWidget::resizeEvent(QResizeEvent *event) {
    qDebug() << "resize" << width() << height();
#ifdef USE_VideoWidget
    backgroundWidget->setGeometry(0, 0, width(), height());
    resizeVideo();
#endif
#ifdef USE_GraphicsScene
    view->setGeometry(0, 0, width(), height());
    resizeVideo();
    resizeBackground();
#endif
}

void VideoWidget::slotMetaDataChanged(const QString &key, const QVariant &value) {
    //qDebug() << "slotMetaDataChanged" << key << value;
    if (key == "Resolution") {
        QSize size = value.toSize();
        videoWidth = size.width();
        videoHeight = size.height();
        resizeVideo();
    }
}

void VideoWidget::slotMetaDataAvailableChanged(bool available) {
    //qDebug() << "slotMetaDataAvsilableChanged" << available;
    if (available) {
        QVariant s = mediaPlayer->metaData("Resolution");
        if (s.isValid()) {
            QSize size = mediaPlayer->metaData("Resolution").toSize();
            videoWidth = size.width();
            videoHeight = size.height();
            resizeVideo();
        }
    }
}

#ifdef USE_VideoWidget
void VideoWidget::resizeVideo() {
    if (videoWidth <= 0 || videoHeight <= 0) {
        videoWidget->setGeometry(0, 0, width(), height());
    } else {
        float imageRatio = (float)videoWidth / videoHeight;
        float widgetRatio = (float)width() / height();
        if(imageRatio > widgetRatio) {
            int newWidth = (int)(height() * imageRatio);
            int offset = (newWidth - width()) / -2;
            //qDebug() << "setGeometry" << offset << 0 << newWidth << height();
            videoWidget->setGeometry(offset, 0, newWidth, height());
        } else {
            int newHeight = (int)(width() / imageRatio);
            int offset = (newHeight - height()) / -2;
            //qDebug() << "setGeometry" << 0 << offset << width() << newHeight;
            videoWidget->setGeometry(0, offset, width(), newHeight);
        }
    }
    //qDebug() << "Geometry" << videoWidget->geometry().x() << videoWidget->geometry().y() << videoWidget->geometry().width() << videoWidget->geometry().height();
}
#endif
#ifdef USE_GraphicsScene
void VideoWidget::resizeVideo() {
    if (item->isVisible()) {
        if (videoWidth <= 0 || videoHeight <= 0) {
        } else {
            item->setSize(item->nativeSize());
            //view->fitInView(item, Qt::KeepAspectRatio); //uses margin=2
            double ratio = (double)(width()+2) / (double)videoWidth;
            double yRatio = (double)(height()+2) / (double)videoHeight;
            if (yRatio > ratio) {
                ratio = yRatio;
            }
            if (ratio == 0) {
                ratio = 1;
            }
            qDebug() << seat << "video size" << geometry() << videoWidth << videoHeight << "=" << ratio;
            view->resetTransform();
            view->scale(ratio, ratio);
            view->centerOn(QPoint(videoWidth / 2, videoHeight / 2));
        }
    }
}
#endif

#ifdef USE_GraphicsScene
void VideoWidget::resizeBackground() {
    if (background->isVisible()) {
        //view->fitInView(background, Qt::KeepAspectRatioByExpanding); uses margin=2
        QSize size = background->pixmap().size();
        double ratio = (double)(width()+2) / (double)size.width();
        double yRatio = (double)(height()+2) / (double)size.height();
        if (yRatio > ratio) {
            ratio = yRatio;
        }
        if (ratio == 0) {
            ratio = 1;
        }
        qDebug() << seat << "background size" << geometry() << size << "=" << ratio;
        view->resetTransform();
        view->scale(ratio, ratio);
        view->centerOn(QPoint(size.width() / 2, size.height() / 2));
    }
}
#endif

void VideoWidget::slotPlayerSelected(int s, QString fn) {
    if (seat != s) {
        return;
    }
    //qDebug() << ">>> slotPlayerSelected" << s << fn;
    active = false;
    checkVideoQueue();

    if (fn.isEmpty()) {
#ifdef USE_VideoWidget
        backgroundWidget->setVisible(true);
        videoWidget->setVisible(false);
#endif
#ifdef USE_GraphicsScene
        background->show();
        item->hide();
        resizeBackground();
#endif
    } else {
        readTimestampFile(fn);

        QString avi(tsFileName);
        avi.replace(Util::regPatternTs, ".avi");
        mediaPlayer->setMedia(QUrl::fromLocalFile(avi));
        active = true;

#ifdef USE_VideoWidget
        backgroundWidget->setVisible(false);
        videoWidget->setVisible(true);
#endif
#ifdef USE_GraphicsScene
        item->show();
        background->hide();
        resizeVideo();
#endif
        enqueue(JobActionIntro, 4);
    }
}

void VideoWidget::readTimestampFile(QString file) {
    tsFileName = file;

    for (int t = 0; t < 7; t++) {
        for (int i = 0; i < 5; i++) {
            QMap<QString, QVector<ClipTimes> >::iterator c;
            for (c = videoClips[t][i].begin(); c != videoClips[t][i].end(); c++) {
                c.value().clear();
            }
        }
    }
    QFile inFile(tsFileName);
    if (!inFile.open(QIODevice::ReadOnly)) {
        emit fatalError(QString("Can't open %1: %2").arg(tsFileName).arg(inFile.errorString()));
        return;
    }
    QTextStream in(&inFile);
    qint64 startTime = -1L;
    QString startAction;
    int startPieces = -1;
    ClipSound startQuiet = ClipSoundLoud;
    bool startZoom = false;
    int lineNr = 0;
    QString buf;
    QString startBuf;
    while (in.readLineInto(&buf)) {
        lineNr++;
        //qDebug() << lineNr << ": " << buf;
        qint64 time;
        QString action;
        int pieces;
        QString modifier;
        ClipSound quiet;
        bool zoom;
        TimestampLine res = Util::scanTimestampLine(buf, &time, &action, &pieces, &modifier, &zoom, &quiet);

        if (res == TimestampLineComment || res == TimestampLineWhitespace) {
        } else if (res != TimestampLineAction) {
            emit error(QString("Illegal line %1 in %2: %3").arg(lineNr).arg(tsFileName).arg(buf));
        } else {
            if (!modifier.isEmpty()) {
                action = action + "_" + modifier;
            }

            if (startTime >= 0) {
                if (startAction == "-") {
                } else if (!videoClips[ClipType::ALL][startPieces].contains(startAction)) {
                    emit error(QString("Illegal action, line %1 in %2: %3").arg(lineNr-1).arg(tsFileName).arg(startBuf));
                } else if (time <= startTime + 500) {
                    emit error(QString("Clip to short, line %1 in %2: %3").arg(lineNr-1).arg(tsFileName).arg(startBuf));
                } else {
                    if (startAction == "broke") {
                        startZoom = true;
                    }
                    QVector<ClipTimes> &tv1 = videoClips[ClipType::ALL][startPieces][startAction];
                    tv1.append(ClipTimes(startTime, time, startQuiet));
                    QVector<ClipTimes> &tv2 = videoClips[startZoom ? ClipType::ZOOM_IN : ClipType::ZOOM_OUT][startPieces][startAction];
                    tv2.append(ClipTimes(startTime, time, startQuiet));
                    if (startQuiet) {
                        QVector<ClipTimes> &tv3 = videoClips[startZoom ? ClipType::QUIET_ZOOM : ClipType::QUIET][startPieces][startAction];
                        tv3.append(ClipTimes(startTime, time, startQuiet));
                        QVector<ClipTimes> &tv4 = videoClips[startZoom ? ClipType::LOUD_ZOOM : ClipType::LOUD][startPieces][startAction];
                        tv4.append(ClipTimes(startTime, time, startQuiet));
                    } else {
                        QVector<ClipTimes> &tv4 = videoClips[startZoom ? ClipType::LOUD_ZOOM : ClipType::LOUD][startPieces][startAction];
                        tv4.append(ClipTimes(startTime, time, startQuiet));
                    }
                    //qDebug() << "Add clip " << startPieces << " " << startAction << " " << startTime << " " << time << "to:" << startQuiet << startZoom << type;
                }
            }
            startBuf = buf;
            startTime = time;
            startAction = action;
            startPieces = pieces;
            startQuiet = quiet;
            startZoom = zoom;
        }
    }
    idle = false;
    filler = false;

    pieces = 4;
    hasCards = false;
    hasGoodCards = false;
    hasBadCards = false;
    isDead = false;
    drinksTaken = 0;
}

void VideoWidget::slotPlayerAction(int s, JobAction action, int newPieces, bool needsSignalDone) {
    if (s != -1 && s != seat) {
        return;
    }
    //qDebug() << ">>>  " << seat << Util::nameOfAction(action) << newPieces << needsSignalDone;
    if (newPieces < 0) {
        newPieces = pieces;
    }
    if (action == JobActionZoomIn) {
        tryZoom = ClipType::ZOOM_IN;
        tryLoud[0] = ClipType::LOUD_ZOOM;
        tryLoud[1] = ClipType::LOUD;
        tryQuiet[0] = ClipType::QUIET_ZOOM;
        tryQuiet[1] = ClipType::QUIET;
    }
    if (action == JobActionZoomOut) {
        tryZoom = ClipType::ZOOM_OUT;
        tryLoud[0] = ClipType::LOUD;
        tryLoud[1] = ClipType::LOUD_ZOOM;
        tryQuiet[0] = ClipType::QUIET;
        tryQuiet[1] = ClipType::QUIET_ZOOM;
    }
    bool take = false;
    bool out = false;
    JobAction todo;
    if (active) {
        if (action == JobActionWin) {
            if (hasCards) {
                enqueue(JobActionWin, pieces);
                hasCards = false;
            }
       } else if (action == JobActionWinYoulose) {
            if (hasCards) {
                enqueue(JobActionWinYoulose, pieces);
                hasCards = false;
            }
        } else if (action == JobActionLose) {
            if (hasCards) {
                enqueue(JobActionLose, pieces);
                hasCards = false;
            }
        } else if (action == JobActionLoseYouWin) {
            if (hasCards) {
                enqueue(JobActionLoseYouWin, pieces);
                hasCards = false;
            }
        } else if (action == JobActionTake) {
            if (!hasCards) {
                take = true;
                hasCards = true;
                hasGoodCards = false;
                hasBadCards = false;
                todo = JobActionTake;
            }
        } else if (action == JobActionTakeGood) {
            if (!hasCards) {
                take = true;
                hasCards = true;
                hasGoodCards = false;
                hasBadCards = false;
                todo = JobActionTakeGood;
            }
        } else if (action == JobActionTakeBad) {
            if (!hasCards) {
                take = true;
                hasCards = true;
                hasGoodCards = false;
                hasBadCards = true;
                todo = JobActionTakeBad;
            }
        } else if (action == JobActionCards) {
            hasGoodCards = false;
            hasBadCards = false;
            if (hasCards) {
                if (videoQueue.isEmpty()) {
                    enqueue(JobActionCards, pieces);
                } else {
                    Job last = videoQueue.last();
                    if (last.action != JobActionCards && last.action != JobActionCardsGood && last.action != JobActionCardsBad) {
                        enqueue(JobActionCards, pieces); //FIXME repace last state
                    }
                }
            }
        } else if (action == JobActionCardsGood) {
            hasGoodCards = false;
            hasBadCards = false;
            if (hasCards) {
                if (videoQueue.isEmpty()) {
                    enqueue(JobActionCardsGood, pieces);
                } else {
                    Job last = videoQueue.last();
                    if (last.action != JobActionCards && last.action != JobActionCardsGood && last.action != JobActionCardsBad) {
                        enqueue(JobActionCardsGood, pieces);
                    }
                }
            }
        } else if (action == JobActionCardsBad) {
            hasGoodCards = false;
            hasBadCards = true;
            if (hasCards) {
                if (videoQueue.isEmpty()) {
                    enqueue(JobActionCardsBad, pieces);
                } else {
                    Job last = videoQueue.last();
                    if (last.action != JobActionCards && last.action != JobActionCardsGood && last.action != JobActionCardsBad) {
                        enqueue(JobActionCardsBad, pieces);
                    }
                }
            }
        } else if (action == JobActionFold) {
            if (hasCards) {
                enqueue(JobActionFold, pieces);
                hasCards = false;
            }
        } else if (action == JobActionShow) {
            if (videoQueue.isEmpty()) {
                enqueue(JobActionShow, pieces);
            } else {
                Job last = videoQueue.last();
                if (last.action != JobActionShow && last.action != JobActionDrink) {
                    enqueue(JobActionShow, pieces);
                }
            }
        } else if (action == JobActionDrink) {
            if (videoQueue.isEmpty()) {
                enqueue(JobActionDrink, pieces);
            } else {
                Job last = videoQueue.last();
                if (last.action != JobActionDrink) {
                    enqueue(JobActionDrink, pieces);
                }
            }
        } else if (action == JobActionNewGame) {
            isDead = false;
            gameOver = false;
        } else if (action == JobActionGameOver) {
            gameOver = true;
        } else if (action == JobActionDead) {
            out = true;
            hasCards = false;
        }

        while (pieces > newPieces) {
            pieces--;
            while (pieces > 0 && videoClips[ClipType::ALL][pieces]["off"].isEmpty()) {
                pieces--;
            }
            enqueue(JobActionOff, pieces);
        }
        int tmpPieces = pieces;
        while (tmpPieces < newPieces) {
            tmpPieces++;
            if (tmpPieces == 4 || !videoClips[ClipType::ALL][tmpPieces]["on"].isEmpty()) {
                pieces = tmpPieces;
                enqueue(JobActionOn, pieces);
            }
        }

        if (take) {
            enqueue(todo, pieces);
        }

        if (out && !isDead) {
            enqueue(JobActionDead, pieces);
            isDead = true;
        }
        if (needsSignalDone) {
            enqueue(JobActionSignal, pieces);
        }
    } else if (needsSignalDone) {
        signalPlayerActionDone(seat);
    }
}

void VideoWidget::enqueue(JobAction todo, int pieces) {
    if (active) {
        videoQueue.enqueue(Job(todo, pieces));
        //checkVideoQueue();
        sequenceTimer->start(rand() % 100); // random for activeLoudClip
    }
}

void VideoWidget::slotPositionChanged(qint64 position) {
    if (active) {
        if (needsStart) {
            if (position > startTimestamp - 100 && position < startTimestamp + 100) {
                qDebug() << "mediaPlayer start" << position << startTimestamp << "->" << endTimestamp << "=" << mediaPlayer->position();
                needsStart = false;
                QTimer::singleShot(0, mediaPlayer, SLOT(play()));
            } else {
                qDebug() << "mediaPlayer no start" << position;
            }
        } else if (playing) {
            if (position >= endTimestamp - (1000 / 25)) {
                qDebug() << "mediaPlayer position" << position;
                playing = false;
                stopTimer->start(0);
            }
        }
    }
}

void VideoWidget::slotStopVideo() {
    qint64 p1 = mediaPlayer->position();
    mediaPlayer->pause();
    mediaPlayer->setPosition(endTimestamp);
    qint64 p2 = mediaPlayer->position();
    qDebug() << "mediaPlayer stop" << p1 << endTimestamp << "=" << p2;
    if (playingLoud) {
        mutex.lock();
        activeLoudClips--;
        //qDebug() << seat << "end: activeLoudClips--" << activeLoudClips;
        mutex.unlock();
        playingLoud = false;
    }
    doneTimer->start(100);
}

void VideoWidget::slotMediaPlayerError(QMediaPlayer::Error err) {
    emit fatalError(QString("Error in Mediaplayer: %1").arg(mediaPlayer->errorString()));
}

void VideoWidget::slotTimerElapsed() {
    //qDebug() << "Video Timer Elapsed";
    idle = true;
    checkVideoQueue();
}

void VideoWidget::checkVideoQueue() {
    if (!active) {
        mediaPlayer->stop();
        idleTimer->stop();
        doneTimer->stop();
        playing = false;
        if (playingLoud) {
            mutex.lock();
            activeLoudClips--;
            //qDebug() << seat << "load: activeLoudClips--" << activeLoudClips;
            mutex.unlock();
            playingLoud = false;
        }
        while (!videoQueue.isEmpty()) {
            Job job = videoQueue.dequeue();
            if (job.action == JobActionSignal) {
                signalPlayerActionDone(seat);
            }
        }
        return;
    }

    //qDebug() << ">>> checkVideoQueue" << seat << playing << filler << idle << videoQueue.isEmpty();

    if (playing && (!filler || playingLoud)) {
        return;
    }

    if (playing) {
        doneTimer->stop();
        mediaPlayer->pause();
        playing = false;
        if (playingLoud) {
            mutex.lock();
            activeLoudClips--;
            //qDebug() << seat << "end: activeLoudClips--" << activeLoudClips;
            mutex.unlock();
            playingLoud = false;
        }
    }

    if (videoQueue.isEmpty()) {
        if (idle) {
            if (isDead) {
                startVideo(pieces, "broke", "show", true, true);
            } else if (hasCards) {
                if (hasGoodCards) {
                    startVideo(pieces, "show_cards", "show_high", true, true);
                } else if (hasBadCards) {
                    startVideo(pieces, "show_cards", "show_low", true, true);
                } else {
                    startVideo(pieces, "show_cards", true, true);
                }
            } else {
                startVideo(pieces, "show", true, true);
            }
        }
    } else {
        while ((!playing) && !videoQueue.isEmpty()) {
            Job job = videoQueue.dequeue();
            //qDebug() << "Action:" << seat << "=" << job.action << "(" << job.pieces << ")";
            if (job.action == JobActionSignal) {
                signalPlayerActionDone(seat);
            } else if (job.action == JobActionIntro) {
                startVideo(job.pieces, "intro", false, false);
            } else if (job.action == JobActionTake) {
                if (videoClips[tryZoom][job.pieces]["take"].isEmpty()) {
                    startVideo(job.pieces, "take_good", "take_bad", false, false);
                } else {
                    startVideo(job.pieces, "take", false, false);
                }
            } else if (job.action == JobActionTakeGood) {
                if (videoClips[tryZoom][job.pieces]["take_good"].isEmpty()) {
                    startVideo(job.pieces, "take", false, false);
                } else {
                    startVideo(job.pieces, "take_good", false, false);
                }
            } else if (job.action == JobActionTakeBad) {
                if (videoClips[tryZoom][job.pieces]["take_bad"].isEmpty()) {
                    startVideo(job.pieces, "take", false, false);
                } else {
                    startVideo(job.pieces, "take_bad", false, false);
                }
            } else if (job.action == JobActionCards) {
                if (videoClips[tryZoom][job.pieces]["show_cards"].isEmpty()) {
                    startVideo(job.pieces, "show_high", "show_low", false, true);
                } else {
                    startVideo(job.pieces, "show_cards", false, true);
                }
            } else if (job.action == JobActionCardsGood) {
                if (videoClips[tryZoom][job.pieces]["show_high"].isEmpty()) {
                    startVideo(job.pieces, "show_cards", false, true);
                } else {
                    startVideo(job.pieces, "show_high", false, true);
                }
            } else if (job.action == JobActionCardsBad) {
                if (videoClips[tryZoom][job.pieces]["show_low"].isEmpty()) {
                    startVideo(job.pieces, "show_cards", false, true);
                } else {
                    startVideo(job.pieces, "show_low", false, true);
                }
            } else if (job.action == JobActionFold) {
                startVideo(job.pieces, "drop_throw", "drop", false, false);
            } else if (job.action == JobActionWin) {
                startVideo(job.pieces, "win", false, false);
            } else if (job.action == JobActionWinYoulose) {
                startVideo(job.pieces, "win", "win_youlose", false, false);
            } else if (job.action == JobActionLose) {
                if ( videoClips[tryZoom][pieces]["lose"].isEmpty()) {
                    startVideo(job.pieces, "drop", false, false);
                } else {
                    startVideo(job.pieces, "lose", false, false);
                }
            } else if (job.action == JobActionLoseYouWin) {
                int cnt = videoClips[tryZoom][job.pieces]["lose"].size() + videoClips[tryZoom][job.pieces]["lose_youwin"].size();
                if (cnt == 0) {
                    startVideo(job.pieces, "drop", false, false);
                } else {
                    startVideo(job.pieces, "lose_youwin", "lose", false, false);
                }
            } else if (job.action == JobActionDead) {
                startVideo(job.pieces, "broke", false, false);
            } else if (job.action == JobActionOn) {
                startVideo(job.pieces, "on", false, false);
            } else if (job.action == JobActionOff) {
                startVideo(job.pieces, "off", false, false);
            } else if (job.action == JobActionShow) {
                startVideo(job.pieces, "show", false, true);
            } else if (job.action == JobActionDrink) {
                int p = rand() % 20;
                if (drinksTaken > p) {
                    if (hasCards) {
                        if (!videoClips[tryZoom][job.pieces]["no_cards"].isEmpty()) {
                            startVideo(job.pieces, "no_cards", false, false);
                        } else if (!videoClips[tryZoom][job.pieces]["no"].isEmpty()) {
                            startVideo(job.pieces, "no", false, false);
                        } else {
                            startVideo(job.pieces, "show_drink", false, false);
                            drinksTaken++;
                        }
                    } else {
                        if (!videoClips[tryZoom][job.pieces]["no"].isEmpty()) {
                            startVideo(job.pieces, "no", false, false);
                        } else {
                            startVideo(job.pieces, "show_drink", false, false);
                            drinksTaken++;
                        }
                    }
                } else {
                    startVideo(job.pieces, "show_drink", false, false);
                    drinksTaken++;
                }
            }
        }
    }

    if (playing) {
        idleTimer->stop();
    } else if (!idleTimer->isActive()) {
        if (gameOver) {
            idleTimer->start(2000);
        } else if (isDead) {
            idleTimer->start(10000);
        } else if (hasCards) {
            idleTimer->start(5000);
        } else {
            idleTimer->start(8000);
        }
    }
}

void VideoWidget::startVideo(int pieces, QString action1, bool dispensable  ,bool fill) {
    startVideo(pieces, action1, "none", "none", dispensable, fill);
}
void VideoWidget::startVideo(int pieces, QString action1, QString action2, bool dispensable, bool fill) {
    startVideo(pieces, action1, action2, "none", dispensable, fill);
}
void VideoWidget::startVideo(int pieces, QString action1, QString action2, QString action3, bool dispensable, bool fill) {
    QVector<ClipTimes> clips1;
    QVector<ClipTimes> clips2;
    QVector<ClipTimes> clips3;
    // assert playingLoud == false;

    bool playingZoom = false;

    mutex.lock();

    if (activeLoudClips == 0) {
        clips1 = videoClips[tryLoud[0]][pieces][action1];
        clips2 = videoClips[tryLoud[0]][pieces][action2];
        clips3 = videoClips[tryLoud[0]][pieces][action3];
        if (clips1.length() + clips2.length() + clips3.length() > 0) {
            //qDebug() << "    :" << seat << "activeLoudClips++" << activeLoudClips;
            activeLoudClips++;
            playingLoud = true;
            playingZoom = tryZoom == ClipType::ZOOM_IN;
        } else if (!dispensable) {
            clips1 = videoClips[tryLoud[1]][pieces][action1];
            clips2 = videoClips[tryLoud[1]][pieces][action2];
            clips3 = videoClips[tryLoud[1]][pieces][action3];
            if (clips1.length() + clips2.length() + clips3.length() > 0) {
                //qDebug() << "    :" << seat << "no zoom loud clip found, using other loud";
                activeLoudClips++;
                playingLoud = true;
            } else {
                //qDebug() << "    :" << seat << "no zoom loud clip found, using other quiet";
            }
            playingZoom = tryZoom != ClipType::ZOOM_IN;
        }
    } else {
        clips1 = videoClips[tryQuiet[0]][pieces][action1];
        clips2 = videoClips[tryQuiet[0]][pieces][action2];
        clips3 = videoClips[tryQuiet[0]][pieces][action3];
        if (clips1.length() + clips2.length() + clips3.length() > 0) {
            //qDebug() << "    :" << seat << "quiet clip found";
            playingZoom = tryZoom == ClipType::ZOOM_IN;
        } else if (!dispensable) {
            clips1 = videoClips[tryLoud[0]][pieces][action1];
            clips2 = videoClips[tryLoud[0]][pieces][action2];
            clips3 = videoClips[tryLoud[0]][pieces][action3];
            if (clips1.length() + clips2.length() + clips3.length() > 0) {
                activeLoudClips++;
                playingLoud = true;
                playingZoom = tryZoom == ClipType::ZOOM_IN;
                //qDebug() << "    :" << seat << "no quiet clip found, using loud";
            } else {
                clips1 = videoClips[tryQuiet[1]][pieces][action1];
                clips2 = videoClips[tryQuiet[1]][pieces][action2];
                clips3 = videoClips[tryQuiet[1]][pieces][action3];
                if (clips1.length() + clips2.length() + clips3.length() > 0) {
                    //qDebug() << "    :" << seat << "no zoom quiet clip found, using other";
                } else {
                    clips1 = videoClips[tryLoud[1]][pieces][action1];
                    clips2 = videoClips[tryLoud[1]][pieces][action2];
                    clips3 = videoClips[tryLoud[1]][pieces][action3];
                    //qDebug() << "    :" << seat << "no zoom quiet clip found, using loud";
                    if (clips1.length() + clips2.length() + clips3.length() > 0) {
                        activeLoudClips++;
                        playingLoud = true;
                    }
                }
                playingZoom = tryZoom != ClipType::ZOOM_IN;
            }
        }
    }

    mutex.unlock();

    //qDebug() << "Play:" << seat << action1 << action2 << action3 << pieces << fill << "activeloud:" << activeLoudClips << playingLoud << "zoom:" << (tryLoud[0] == ClipType::LOUD_ZOOM) << playingZoom << "variety:" << (clips1.length() + clips2.length() + clips3.length());

    if (clips1.length() + clips2.length() + clips3.length() == 0) {
        //qDebug() << "    :" << seat << " no clip found";
        if (!dispensable) {
            emit error(QString("No clip for %1 %2/%3/%4 in %5").arg(pieces).arg(action1).arg(action2).arg(action3).arg(tsFileName));
        }
        return;
    }

    bool ok = false;
    int p;
    QString action;
    QVector<ClipTimes> clips;
    for (int i = 0; i < 20; i++) {
        int cnt = clips1.size() + clips2.size() + clips3.size();
        int idx = rand() % cnt;
        if (idx < clips1.size()) {
            clips = clips1;
            action = action1;
        } else if (idx < clips1.size() + clips2.size()) {
            clips = clips2;
            action = action2;
        } else {
            clips = clips3;
            action = action3;
        }
        p = rand() % clips.length();
        ok = !lruClips.contains(clips[p].start);

        if (ok && i > 20 - (clips[p].end - clips[p].start) / 100 ) {
            break;
        }
    }
    if (!ok && dispensable) {
        //qDebug() << "    :" << seat << " no new clip found for dispansable";
        if (playingLoud) {
            mutex.lock();
            activeLoudClips--;
            //qDebug() << seat << "end: activeLoudClips--" << activeLoudClips;
            mutex.unlock();
            playingLoud = false;
        }
        for (int i  = rand() % 2 + 1; i >= 0 && !lruClips.isEmpty(); i--) {
            lruClips.dequeue();
        }
        return;
    }
    if (playingLoud && clips[p].quiet) {
        mutex.lock();
        activeLoudClips--;
        //qDebug() << seat << "end: activeLoudClips--" << activeLoudClips;
        mutex.unlock();
        playingLoud = false;
    }
    lruClips.enqueue(clips[p].start);
    while(lruClips.length() > 5) {
        for (int i  = rand() % 2 + 1; i >= 0; i--) {
            lruClips.dequeue();
        }
    }
    int h = clips[p].start / (60 * 60 * 1000);
    int min = (clips[p].start % (60 * 60 * 1000)) / (60 * 1000);
    int sec = (clips[p].start % (60 * 1000)) / 1000;
    int ms = (clips[p].start % 1000);
    //qDebug() << "    :" << seat << QString::asprintf("%02d:%02d:%02d:%03d", h, min, sec, ms) << QString("%1[%2]").arg(action).arg(p) << pieces << fill << "loud:" << playingLoud;
    QByteArray a = action.toLatin1();
    emit info(QString::asprintf("Play %d %02d:%02d:%02d:%03d %s/%d %s", seat, h, min, sec, ms, a.data(), pieces, (fill ? " (idle)" : "")));
    int len = clips[p].end - clips[p].start;
    if (len < 100) {
       len = 100;
    }
    startTimestamp = clips[p].start;
    endTimestamp = clips[p].end - (1000 / 25);
    mediaPlayer->pause();
    qint64 p1 = mediaPlayer->position();
    mediaPlayer->setMuted(clips[p].quiet == ClipSoundMute);
    mediaPlayer->setPosition(clips[p].start);
    qint64 p2 = mediaPlayer->position();
    //mediaPlayer->play();
    //QTimer::singleShot(100, mediaPlayer, SLOT(play()));
    qint64 p3 = mediaPlayer->position();
    qDebug() << "mediaPlayer setposition" << p1 << clips[p].start << "->" << clips[p].end << "=" << p2 << p3;
    playing = true;
    idle = false;
    filler = fill;
    needsStart = true;
}

QMutex VideoWidget::mutex;
int VideoWidget::activeLoudClips = 0;
