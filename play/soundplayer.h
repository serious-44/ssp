#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QObject>
#include <QVector>
#include <QQueue>
#include <QMediaPlayer>

#include "util.h"

class SoundPlayer : public QObject
{
    Q_OBJECT
public:
    explicit SoundPlayer(QObject *parent = 0);
    ~SoundPlayer();

signals:
    void fatalError(QString log);
    void error(QString log);
    void info(QString log);

public slots:
    void slotPlayerAction(int seat, JobAction action, int pieces, bool needsSignalDone);
    void mediaError(QMediaPlayer::Error error);
    void mediaStateChanged(QMediaPlayer::State state);
    void checkQueue();
    void enqueueLaugh();
    void enqueueClap();

private:
    void enqueue(const QVector<QString> &files);

    QMediaPlayer *mediaPlayer1;
    QMediaPlayer *mediaPlayer2;
    QMediaPlayer *mediaPlayer3;
    QMediaPlayer *mediaPlayer4;

    QVector<int> stats;
    QQueue<QString> soundQueue;

    static QVector<QString> cardPlace;
    static QVector<QString> cardSlide;
    static QVector<QString> chipsLay;
    static QVector<QString> chipsHandle;
    static QVector<QString> huh;
    static QVector<QString> laugh;
    static QVector<QString> yes;
    static QVector<QString> clap;
};

#endif // SOUNDPLAYER_H
