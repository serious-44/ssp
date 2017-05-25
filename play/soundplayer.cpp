#include "soundplayer.h"

#include <cstdlib>

#include <QDebug>
#include <QTimer>

SoundPlayer::SoundPlayer(QObject *parent) :
    QObject(parent),
    stats(31, 0)
{
    mediaPlayer1 = new QMediaPlayer();
    connect(mediaPlayer1, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(mediaError(QMediaPlayer::Error)));
    connect(mediaPlayer1, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(mediaStateChanged(QMediaPlayer::State)));
    mediaPlayer2 = new QMediaPlayer();
    connect(mediaPlayer2, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(mediaError(QMediaPlayer::Error)));
    connect(mediaPlayer2, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(mediaStateChanged(QMediaPlayer::State)));
    mediaPlayer3 = new QMediaPlayer();
    connect(mediaPlayer3, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(mediaError(QMediaPlayer::Error)));
    connect(mediaPlayer3, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(mediaStateChanged(QMediaPlayer::State)));
    mediaPlayer4 = new QMediaPlayer();
    connect(mediaPlayer4, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(mediaError(QMediaPlayer::Error)));
    connect(mediaPlayer4, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(mediaStateChanged(QMediaPlayer::State)));
}

SoundPlayer::~SoundPlayer() {
    mediaPlayer1->deleteLater();
    mediaPlayer2->deleteLater();
    mediaPlayer3->deleteLater();
    mediaPlayer4->deleteLater();
}

void SoundPlayer::slotPlayerAction(int seat, JobAction action, int pieces, bool needsSignalDone) {
    qDebug() << "soundPlayer" << seat << Util::nameOfAction(action);
    stats[action]++;

    if (action == JobActionNewGame) {
    } else if (action == JobActionNewHand) {
        enqueue(chipsLay);
    } else if (action == JobActionTake || action == JobActionTakeGood || action == JobActionTakeBad) {
        enqueue(cardSlide);
        enqueue(cardSlide);
    } else if (action == JobActionDealFlopCards) {
        enqueue(cardPlace);
        enqueue(cardPlace);
        enqueue(cardPlace);
    } else if (action == JobActionDealTurnCards) {
        enqueue(cardPlace);
    } else if (action == JobActionDealRiverCards) {
        enqueue(cardPlace);
    } else if (action == JobActionCall || action == JobActionBet || action == JobActionAllIn) {
        enqueue(chipsLay);
    } else if (action == JobActionWin || action == JobActionWinYoulose) {
        enqueue(chipsHandle);
    } else if (action == JobActionIntro && seat == 0) {
        enqueue(yes);
        QTimer::singleShot(400 + rand() % 200, this, SLOT(enqueueClap()));
    } else if (action == JobActionDead && seat == 0) {
        enqueue(huh);
        QTimer::singleShot(400 + rand() % 200, this, SLOT(enqueueLaugh()));
    } else if (action == JobActionGameOver) {
        for ( int i = 1; i < stats.length(); i++) {
            emit info(QString("%1 = %2").arg(Util::nameOfAction((JobAction)i)).arg(stats[i]));
        }
    }

}

void SoundPlayer::enqueueLaugh() {
    for (int i = 0; i < 5; i++) {
        enqueue(laugh);
    }
}

void SoundPlayer::enqueueClap() {
    for (int i = 0; i < 3; i++) {
        enqueue(clap);
    }
}

void SoundPlayer::mediaError(QMediaPlayer::Error error) {
    emit fatalError(QString("Sound error %1").arg(error));
}

void SoundPlayer::mediaStateChanged(QMediaPlayer::State state) {
    //qDebug() << "mediaplayer" << state;
    if (state == QMediaPlayer::StoppedState) {
        QTimer::singleShot(0, this, SLOT(checkQueue()));
    }
}

void SoundPlayer::enqueue(const QVector<QString> &files) {
    int pos = rand() % files.length();
    soundQueue.enqueue(files[pos]);
    checkQueue();
}

void SoundPlayer::checkQueue() {
    //qDebug() << "check" << soundQueue.isEmpty() << mediaPlayer->state();
    if (!soundQueue.isEmpty()) {
        int volume;
        QString head = soundQueue.head();
        if (head.contains("card")) {
            volume = 20 + rand() % 40;
        } else if (head.contains("huh_") || head.contains("yes_")) {
            volume = 100;
        } else if (head.contains("Coins_Pouring")) {
            volume = 70 + rand() % 20;
        } else {
            volume = 40 + rand() % 20;
        }
        if (mediaPlayer1->state() == QMediaPlayer::StoppedState) {
            QString file = soundQueue.dequeue();
            qDebug() << "play1" << file << volume;
            mediaPlayer1->setVolume(volume);
            mediaPlayer1->setMedia(QUrl(file));
            mediaPlayer1->play();
            QTimer::singleShot(rand() % 200, this, SLOT(checkQueue()));
        } else if (mediaPlayer2->state() == QMediaPlayer::StoppedState) {
            QString file = soundQueue.dequeue();
            qDebug() << "play2" << file << volume;
            mediaPlayer2->setVolume(volume);
            mediaPlayer2->setMedia(QUrl(file));
            mediaPlayer2->play();
            QTimer::singleShot(rand() % 200, this, SLOT(checkQueue()));
        } else if (mediaPlayer3->state() == QMediaPlayer::StoppedState) {
            QString file = soundQueue.dequeue();
            qDebug() << "play3" << file << volume;
            mediaPlayer3->setVolume(volume);
            mediaPlayer3->setMedia(QUrl(file));
            mediaPlayer3->play();
            QTimer::singleShot(rand() % 200, this, SLOT(checkQueue()));
        } else if (mediaPlayer4->state() == QMediaPlayer::StoppedState) {
            QString file = soundQueue.dequeue();
            qDebug() << "play4" << file << volume;
            mediaPlayer4->setVolume(volume);
            mediaPlayer4->setMedia(QUrl(file));
            mediaPlayer4->play();
            QTimer::singleShot(rand() % 200, this, SLOT(checkQueue()));
        }
    }
}

QVector<QString> SoundPlayer::cardPlace {
    "qrc:/sounds/cardPlace1.wav",
    "qrc:/sounds/cardPlace2.wav",
    "qrc:/sounds/cardPlace3.wav",
    "qrc:/sounds/cardPlace4.wav"
};
QVector<QString> SoundPlayer::cardSlide {
    "qrc:/sounds/cardSlide1.wav",
    "qrc:/sounds/cardSlide2.wav",
    "qrc:/sounds/cardSlide3.wav",
    "qrc:/sounds/cardSlide4.wav",
    "qrc:/sounds/cardSlide5.wav",
    "qrc:/sounds/cardSlide6.wav",
    "qrc:/sounds/cardSlide7.wav",
    "qrc:/sounds/cardSlide8.wav"
};
QVector<QString> SoundPlayer::chipsLay {
    "qrc:/sounds/Coins_Several_00.mp3",
    "qrc:/sounds/Coins_Several_01.mp3",
    "qrc:/sounds/Coins_Several_02.mp3",
    "qrc:/sounds/Coins_Several_09.mp3",
    "qrc:/sounds/Coins_Several_11.mp3",
    "qrc:/sounds/Coins_Several_15.mp3",
    "qrc:/sounds/Coins_Several_17.mp3"
};
QVector<QString> SoundPlayer::chipsHandle {
    "qrc:/sounds/Coins_Pouring_00.mp3",
    "qrc:/sounds/Coins_Pouring_02.mp3",
    "qrc:/sounds/Coins_Pouring_03.mp3",
    "qrc:/sounds/Coins_Pouring_10.mp3"
};
QVector<QString> SoundPlayer::huh {
    "qrc:/sounds/huh_1.wav",
    "qrc:/sounds/huh_2.wav"
};
QVector<QString> SoundPlayer::laugh {
    "qrc:/sounds/laugh-10.wav",
    "qrc:/sounds/laugh-6.wav",
    "qrc:/sounds/laugh-8.wav",
    "qrc:/sounds/laugh-9.wav"
};
QVector<QString> SoundPlayer::yes {
    "qrc:/sounds/yes_1.wav",
    "qrc:/sounds/yes_2.wav"
};
QVector<QString> SoundPlayer::clap {
    "qrc:/sounds/applause10.mp3",
    "qrc:/sounds/applause6.mp3",
    "qrc:/sounds/applause8.mp3"
};
