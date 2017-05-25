#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QSet>
#include <QMap>
#include <QSettings>
#include <QVector>

enum TimestampLine {
    TimestampLineInvalid = 0,
    TimestampLineIllegal = 1,
    TimestampLineTimestamp = 2,
    TimestampLineAction = 3,
    TimestampLineComment = 4,
    TimestampLineWhitespace = 5
};

enum JobAction {
    JobActionNewGame = 1,
    JobActionNewHand = 2,
    JobActionGameOver = 3,
    JobActionTake = 4,
    JobActionTakeGood = 5,
    JobActionTakeBad = 6,
    JobActionWin = 7,
    JobActionWinYoulose = 8,
    JobActionLose = 9,
    JobActionLoseYouWin = 10,
    JobActionFold = 11,
    JobActionCheck = 12,
    JobActionCall = 13,
    JobActionBet = 14,
    JobActionRaise = 15,
    JobActionAllIn = 16,
    JobActionDead = 17,
    JobActionDrink = 18,
    JobActionCards = 19,
    JobActionCardsGood = 20,
    JobActionCardsBad = 21,
    JobActionShow = 22,
    JobActionIntro = 23,
    JobActionOn = 24,
    JobActionOff = 25,
    JobActionZoomIn = 26,
    JobActionZoomOut = 27,
    JobActionDealFlopCards = 28,
    JobActionDealTurnCards = 29,
    JobActionDealRiverCards = 30,

    JobActionSignal = 100
};

enum ClipSound {
    ClipSoundLoud = 0,
    ClipSoundQuiet = 1,
    ClipSoundMute = 2
};

class Util
{
public:
    Util();

    static QString timestampToTimecode(qint64 t);
    static QString timestampToFileTimecode(qint64 t);
    static qint64 addFramesToTimestamp(qint64 t, int f, qint64 duration = -1);
    static qint64 timecodeToTimestamp(QRegExp &regTimecode);

    static QRegExp regTimecode;
    static QRegExp regRoughTimecode;
    static QRegExp regTimecodeAction;
    static QRegExp regActionModifier;
    static QRegExp regModifierQuiet;
    static QRegExp regModifierMute;

    static QRegExp regComment;
    static QRegExp regWhitespace;
    static QRegExp regPreviewUrl;
    static QRegExp regPreviewTimestamp;
    static QRegExp regPreviewCrop;

    static QRegExp regPatternVideo;
    static QRegExp regPatternTs;

    static QMap<QString, QSet<QString> > validActions;

    static TimestampLine scanTimestampLine(const QString &line, qint64* time = Q_NULLPTR, QString *action = Q_NULLPTR, int *pieces = Q_NULLPTR, QString *modifier = Q_NULLPTR, bool *zoom = Q_NULLPTR, ClipSound *quiet = Q_NULLPTR);
    static QStringList expandCompilations(const QStringList &fileNamesRaw, QSettings *setting = Q_NULLPTR);
    static QString formatTsLine(const QString &raw);
    static QString formatTsLine(qint64 time, const QString &action, int pieces, const QString& modifier, bool zoom, ClipSound quiet);

    static QString nameOfAction(JobAction action) {
        QVector<QString> names {"None",
                 "JobActionNewGame",
                 "JobActionNewHand",
                 "JobActionGameOver",
                 "JobActionTake",
                 "JobActionTakeGood",
                 "JobActionTakeBad",
                 "JobActionWin",
                 "JobActionWinYoulose",
                 "JobActionLose",
                 "JobActionLoseYouWin",
                 "JobActionFold",
                 "JobActionCheck",
                 "JobActionCall",
                 "JobActionBet",
                 "JobActionRaise",
                 "JobActionAllIn",
                 "JobActionDead",
                 "JobActionDrink",
                 "JobActionCards",
                 "JobActionCardsGood",
                 "JobActionCardsBad",
                 "JobActionShow",
                 "JobActionIntro",
                 "JobActionOn",
                 "JobActionOff",
                 "JobActionZoomIn",
                 "JobActionZoomOut",
                 "JobActionDealFlopCards",
                 "JobActionDealTurnCards",
                 "JobActionDealRiverCards"};
        return names[action];
    }
};


#endif // UTIL_H
