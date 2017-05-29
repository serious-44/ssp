#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include "backgroundview.h"
#include "util.h"

#include <QWidget>
#include <QMap>
#include <QVector>
#include <QQueue>
#include <QTimer>
#include <QCloseEvent>
#include <QMutex>
#include <QMediaPlayer>
#include <QStackedWidget>
#include <QQueue>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>

class Job {
public:
    enum JobAction action;
    int pieces;

    inline Job(enum JobAction j, int p) {
        action = j;
        pieces = p;
    }
};

class ClipTimes {
public:
    long start;
    long end;
    ClipSound quiet;

    inline ClipTimes() {
        start = 0;
        end = 0;
        quiet = ClipSoundLoud;
    }

    inline ClipTimes(long s, long e, ClipSound q) {
        start = s;
        end = e;
        quiet = q;
    }
};


class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = 0);
    ~VideoWidget();
    void init(int seat);
    void readTimestampFile(QString file);
    //void prepareClose();

public slots:
    void slotPlayerSelected(int seat, QString tsFileName);
    void slotPlayerAction(int seat, JobAction action, int pieces, bool needsSignalDone);
    void slotTimerElapsed();
    void slotMetaDataAvailableChanged(bool available);
    void slotMetaDataChanged(const QString &key, const QVariant &value);
    void slotPositionChanged(qint64);
    void slotStopVideo();
    void checkVideoQueue();
    void slotMediaPlayerError(QMediaPlayer::Error err);

private:
    void resizeVideo();
    void enqueue(JobAction todo, int pieces);
    void startVideo(int pieces, QString action1, bool dispensable, bool fill);
    void startVideo(int pieces, QString action1, QString action2, bool dispensable, bool fill);
    void startVideo(int pieces, QString action1, QString action2, QString action3, bool dispensable, bool fill);

signals:
    void signalPlayerActionDone(int seat);
    void fatalError(QString log);
    void error(QString log);
    void info(QString log);

protected:
    virtual void resizeEvent(QResizeEvent *event);

private:
    int seat;
    bool active;
    bool playing;
    bool playingLoud;
    bool idle;
    bool filler;
    bool needsStart;

    int pieces;
    bool hasCards;
    bool hasGoodCards;
    bool hasBadCards;
    bool isDead;
    bool gameOver;

    int drinksTaken;

    int videoWidth;
    int videoHeight;

    QString tsFileName;
    enum ClipType {ALL = 0, ZOOM_OUT = 1, ZOOM_IN = 2, LOUD = 3, LOUD_ZOOM = 4, QUIET = 5, QUIET_ZOOM = 6};
    QMap<QString, QVector<ClipTimes> > videoClips[7][5];
    QVector<ClipTimes> emptyClips;
    QQueue<Job> videoQueue;
    QTimer *idleTimer;
    QTimer *stopTimer;
    QTimer *doneTimer;
    QTimer *sequenceTimer;

    QMediaPlayer *mediaPlayer;
    QGraphicsScene *scene;
    BackgroundView *view;
    QGraphicsVideoItem *item;

    qint64 startTimestamp;
    qint64 endTimestamp;
    QQueue<qint64> lruClips;

    ClipType tryZoom;
    ClipType tryLoud[2];
    ClipType tryQuiet[2];
    static QMutex mutex;
    static int activeLoudClips;
};

#endif // VIDEOWIDGET_H
