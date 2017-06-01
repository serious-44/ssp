#ifndef CLIPEDITOR_H
#define CLIPEDITOR_H

#include <QWidget>
#include <QDebug>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QKeyEvent>
#include <QSlider>
#include <QTextCursor>
#include <QTextDocument>
#include <QSyntaxHighlighter>
#include <QVector>
#include <QAudioDecoder>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QGraphicsPixmapItem>
#include <QRegExp>
#include <QTextBlock>

namespace Ui {
class ClipEditor;
}

class ClipEditor;

class MyHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit MyHighlighter(ClipEditor *e, QTextDocument *parent = 0);
    ~MyHighlighter();

protected:
    virtual void highlightBlock(const QString &text);

private:
    ClipEditor *editor;
};

class ScanAudio {
public:
    qint64 start;
    qint64 end;
    QTextBlock line;
    qint32 audioSamples;
    qint32 audioLoudSamples;

    ScanAudio(qint64 s, qint64 e, QTextBlock &b) {
        start = s;
        end = e;
        line = b;
        audioSamples = 0;
        audioLoudSamples = 0;
    }
    ScanAudio() {
        start = 0;
        end = 0;
        audioSamples = 0;
        audioLoudSamples = 0;
    }
};

class ClipEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ClipEditor(QWidget *parent = 0);
    ~ClipEditor();
    void prepareClose(bool save);
    bool needsSave();

public slots:
    void slotOpenFile();
    void slotSaveTsFile();
    void slotSavePreview();
    void slotPrepareImage();
    //void slotEnableSavePreview();
    void slotTrimPreview(bool on);
    void slotTrimImage(bool on);

    void slotTextChanged();
    void slotTextCursorChanged();

    void slotSliderMoved(int value);
    void slotSliderPressed();
    void slotSliderReleased();
    void slotPlayStop();
    void slotPosStart();
    void slotPosBack();
    void slotPosBack1();
    void slotPosForward1();
    void slotPosForward();
    void slotPosEnd();
    void slotPosGotoMark();
    void slotPosPrevMark();
    void slotPosNextMark();
    void slotPosSnap();

    void slotPieces4();
    void slotPieces3();
    void slotPieces2();
    void slotPieces1();
    void slotPieces0();
    void slotIntro();
    void slotTake();
    void slotDrop();
    void slotWin();
    void slotLose();
    void slotShow();
    void slotOn4();
    void slotOn3();
    void slotOn2();
    void slotOn1();
    void slotOff3();
    void slotOff2();
    void slotOff1();
    void slotOff0();
    void slotDead();
    void slotNo();
    void slotX();

    void slotModGood();
    void slotModBad();
    void slotModThrow();
    void slotModYoulose();
    void slotModYouwin();
    void slotModCards();
    void slotModNoWCards();
    void slotModHigh();
    void slotModLow();
    void slotModDrink();
    void slotTimestamp();
    void slotScanAudio();
    void slotFind();
    void slotCheck();
    void slotGenerateExtra();

    void slotToggleMute();

    void slotPositionChanged(qint64);
    void slotMetaDataAvailableChanged(bool available);
    void slotMetaDataChanged(const QString &key, const QVariant &value);
    void slotDurationChanged(qint64 dur);
    void slotMediaPlayerError(QMediaPlayer::Error err);

    void slotAudioBufferAvailable();
    void slotAudioError(QAudioDecoder::Error);
    void slotAudioFinished();

    void slotDownloadPreview();
    void slotNetworkError(QNetworkReply::NetworkError err);
    void slotNetworkError(QList<QSslError> err);
    void slotNetworkUpdateProgress(qint64 read, qint64 total);
    void slotNetworkFinished();

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

private:
    void insertTsText(QString action, int pieces, QString modifier);
    void insertTsText(QString action, int pieces, QString modifier, bool zoom);
    void videoStop();

    Ui::ClipEditor *ui;
    QString videoFileName;
    QString tsFileName;

    bool inSliderDrag;
    bool inSlotPositionChanged;

    bool active;
    //qint64 currentPosition;
    qint64 duration;

    QMediaPlayer *mediaPlayer;
    QMediaPlayer *mediaPlayerEnd;
    QMediaPlayer *mediaPlayerStart;
    QAudioDecoder audioDecoder;

    int currentPieces;

    MyHighlighter *tsHighlihter;

    QVector<uint> snap2;
    bool snapWasDisabled;

    QVector<ScanAudio> audioTextBlock;
    QVector<ScanAudio> audioActiveTextBlock;
    qint64 audioCurrentFrame;

    QNetworkAccessManager networkAccessManager;
    QNetworkReply *networkReply;
    QGraphicsPixmapItem networkImage;

    QString oldFindString;
    QTextCursor findCursor;
    QRegExp findExpr;

    void showFatalError(QString log);
    void showError(QString log);
    void showInfo(QString log);

    friend class MyHighlighter;
};

#endif // CLIPEDITOR_H
