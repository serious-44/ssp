#include "util.h"

#include <QtGlobal>
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QDir>

Util::Util()
{

}

QRegExp Util::regTimecode("^[ \t]*([0-9][0-9]):([0-9][0-9]):([0-9][0-9]):([0-9][0-9][0-9])");
QRegExp Util::regRoughTimecode("^([ \t]*[0-9:]*[ \t]*)");
QRegExp Util::regTimecodeAction("^[ \t]*([0-9][0-9]):([0-9][0-9]):([0-9][0-9]):([0-9][0-9][0-9])[ \t]+([-a-z]+)[ \t]+([0-4])([ \t]+([a-z]+))?([ \t]+([a-z]+))?([ \t]+([a-z]+))?[ \t]*$");
QRegExp Util::regActionModifier(".*([a-z][ \ta-z]*)");
QRegExp Util::regModifierQuiet("[ \t]quiet");
QRegExp Util::regModifierMute("[ \t]mute");

QRegExp Util::regComment("^[ \t]*#");
QRegExp Util::regWhitespace("^[ \t]*");
QRegExp Util::regPreviewUrl("^#[ \t]*preview-url[ \t]*:[ \t]*([^ \t]+)");
QRegExp Util::regPreviewTimestamp("^#[ \t]*preview-time[ \t]*:[ \t]*([0-9][0-9]):([0-9][0-9]):([0-9][0-9]):([0-9][0-9][0-9])");
QRegExp Util::regPreviewCrop("^#[ \t]*preview-crop[ \t]*:[ \t]*([0-9]+)[ \t,]+([0-9]+)[ \t,]+([0-9]+)[ \t,]+([0-9]+)");

QRegExp Util::regPatternVideo("[.]avi$");
QRegExp Util::regPatternTs("[.]ts$");

QString Util::timestampToTimecode(qint64 t) {
    int h = t / (60 * 60 * 1000);
    int min = (t % (60 * 60 * 1000)) / (60 * 1000);
    int sec = (t % (60 * 1000)) / 1000;
    int ms = (t % 1000);
    int f = (ms * 25 + 100) / 1000;
    return QString::asprintf("%02d:%02d:%02d:%03d %02d", h, min, sec, ms, f);
}

QString Util::timestampToFileTimecode(qint64 t) {
    int h = t / (60 * 60 * 1000);
    int min = (t % (60 * 60 * 1000)) / (60 * 1000);
    int sec = (t % (60 * 1000)) / 1000;
    int ms = (t % 1000);
    return QString::asprintf("%02d:%02d:%02d:%03d", h, min, sec, ms);
}

qint64 Util::addFramesToTimestamp(qint64 t, int df, qint64 duration) {
    qint64 sec = t / 1000;
    int ms = t % 1000;
    int f = (ms * 25 + 100) / 1000;
    f += df;
    while (f < 0) {
        sec--;
        f += 25;
    }
    while (f >= 25) {
        sec++;
        f -= 25;
    }
    qint64 res = sec * 1000 + f * 1000 / 25;
    if (res < 0) {
        res = 0;
    }
    if (duration > 1 && res >= duration) {
        res = duration - 1;
    }
    //qDebug() << t << "+" << df << "=" << res;
    return res;
}

qint64 Util::timecodeToTimestamp(QRegExp &regTimecode) {
    qint64 time = regTimecode.cap(1).toLong() * 60 * 60 * 1000;
    time += regTimecode.cap(2).toLong() * 60 * 1000;
    time += regTimecode.cap(3).toLong() * 1000;
    time += regTimecode.cap(4).toLong();
    return time;
}

QMap<QString, QSet<QString> > Util::validActions {
    {"intro", {"", "quiet", "mute"}},
    {"take", {"", "zoom", "quiet", "mute", "good", "bad"}},
    {"drop", {"", "zoom", "quiet", "mute", "throw"}},
    {"win", {"", "zoom", "quiet", "mute", "youlose"}},
    {"lose", {"", "zoom", "quiet", "mute", "youwin"}},
    {"show", {"", "zoom", "quiet", "mute", "cards", "high", "low", "drink"}},
    {"no", {"", "zoom", "quiet", "mute", "cards"}},
    {"-", {""}},
    {"on", {"", "quiet", "mute"}},
    {"off", {"", "quiet", "mute"}},
    {"broke", {"", "quiet", "mute"}}
};

TimestampLine Util::scanTimestampLine(const QString &line, qint64* time, QString *action, int *pieces, QString *modifier, bool *zoom, ClipSound *quiet) {
    TimestampLine res = TimestampLineInvalid;
    if (regTimecodeAction.indexIn(line) == 0) {
        if (time) {
            *time = timecodeToTimestamp(regTimecodeAction);
        }
        if (action) {
            *action = regTimecodeAction.cap(5);
        }
        if (pieces) {
            *pieces = regTimecodeAction.cap(6).toInt();
        }
        QString mod;
        bool z = false;
        bool q = false;
        bool m = false;
        if (regTimecodeAction.cap(8) == "quiet") {
            q = true;
        } else if (regTimecodeAction.cap(8) == "mute") {
            m = true;
        } else if (regTimecodeAction.cap(8) == "zoom") {
            z = true;
        } else if (regTimecodeAction.cap(8).length() > 0) {
            mod = regTimecodeAction.cap(8);
        }
        if (regTimecodeAction.cap(10) == "quiet") {
            q = true;
        } else if (regTimecodeAction.cap(10) == "mute") {
            m = true;
        } else if (regTimecodeAction.cap(10) == "zoom") {
            z = true;
        } else if (regTimecodeAction.cap(10).length() > 0) {
            mod = regTimecodeAction.cap(10);
        }
        if (regTimecodeAction.cap(12) == "quiet") {
            q = true;
        } else if (regTimecodeAction.cap(12) == "mute") {
            m = true;
        } else if (regTimecodeAction.cap(12) == "zoom") {
            z = true;
        } else if (regTimecodeAction.cap(12).length() > 0) {
            mod = regTimecodeAction.cap(12);
        }
        if (modifier) {
            *modifier = mod;
        }
        if (zoom) {
            *zoom = z;
        }
        if (quiet) {
            if (m) {
                *quiet = ClipSoundMute;
            } else if (q) {
                *quiet = ClipSoundQuiet;
            } else {
                *quiet = ClipSoundLoud;
            }
        }
        res = TimestampLineAction;
        if (validActions.contains(regTimecodeAction.cap(5))) {
            if (!validActions[regTimecodeAction.cap(5)].contains(regTimecodeAction.cap(8))) {
                res = TimestampLineIllegal;
            }
            if (!validActions[regTimecodeAction.cap(5)].contains(regTimecodeAction.cap(10))) {
                res = TimestampLineIllegal;
            }
            if (!validActions[regTimecodeAction.cap(5)].contains(regTimecodeAction.cap(12))) {
                res = TimestampLineIllegal;
            }
        } else {
            res = TimestampLineIllegal;
        }
    } else if (regTimecode.indexIn(line) == 0) {
        if (time) {
            *time = timecodeToTimestamp(regTimecode);
        }
        res = TimestampLineTimestamp;
    } else if (regComment.indexIn(line) == 0) {
        res = TimestampLineComment;
    } else if (regWhitespace.indexIn(line) == 0) {
        res = TimestampLineWhitespace;
    }
    return res;
}

QString Util::formatTsLine(const QString &raw) {
    if (regTimecodeAction.indexIn(raw) == 0) {
        QString buffer;
        QTextStream collect(&buffer);
        collect.setFieldAlignment(QTextStream::AlignLeft);
        QString mod;
        QString quiet;
        QString zoom;
        if (regTimecodeAction.cap(8) == "quiet") {
            quiet = "quiet";
        } else if (regTimecodeAction.cap(8) == "mute") {
            quiet = "mute";
        } else if (regTimecodeAction.cap(8) == "zoom") {
            zoom = "zoom";
        } else if (regTimecodeAction.cap(8).length() > 0) {
            mod = regTimecodeAction.cap(8);
        }
        if (regTimecodeAction.cap(10) == "quiet") {
            quiet = "quiet";
        } else if (regTimecodeAction.cap(10) == "mute") {
            quiet = "mute";
        } else if (regTimecodeAction.cap(10) == "zoom") {
            zoom = "zoom";
        } else if (regTimecodeAction.cap(10).length() > 0) {
            mod = regTimecodeAction.cap(10);
        }
        if (regTimecodeAction.cap(12) == "quiet") {
            quiet = "quiet";
        } else if (regTimecodeAction.cap(12) == "mute") {
            quiet = "mute";
        } else if (regTimecodeAction.cap(12) == "zoom") {
            zoom = "zoom";
        } else if (regTimecodeAction.cap(12).length() > 0) {
            mod = regTimecodeAction.cap(12);
        }
        collect << regTimecodeAction.cap(1) << ":" << regTimecodeAction.cap(2) << ":" << regTimecodeAction.cap(3) << ":" << regTimecodeAction.cap(4);
        collect << " " << qSetFieldWidth(5) <<  regTimecodeAction.cap(5) << qSetFieldWidth(0);
        collect << " " <<  regTimecodeAction.cap(6);
        collect << " " << qSetFieldWidth(7) <<  mod << qSetFieldWidth(0) << " " << qSetFieldWidth(5) << zoom << qSetFieldWidth(5) << quiet << qSetFieldWidth(0);
        return collect.readAll();
    } else {
        return raw;
    }
}

QString Util::formatTsLine(qint64 time, const QString &action, int pieces, const QString& modifier, bool zoom, ClipSound quiet) {
    QString buffer;
    QTextStream collect(&buffer);
    collect.setFieldAlignment(QTextStream::AlignLeft);
    collect << timestampToFileTimecode(time);
    collect << " " << qSetFieldWidth(5) <<  action << qSetFieldWidth(0);
    collect << " " <<  pieces;
    collect << " " << qSetFieldWidth(7) <<  modifier << qSetFieldWidth(0);
    QString qs("");
    if (quiet == ClipSoundMute) {
        qs = "mute";
    }
    if (quiet == ClipSoundQuiet) {
        qs = "quiet";
    }
    collect << " " << qSetFieldWidth(5) << (zoom ? "zoom" : "") << qSetFieldWidth(5) << qs << qSetFieldWidth(0);
    return collect.readAll();
}

QStringList Util::expandCompilations(const QStringList &fileNamesRaw, QSettings *settings) {
    QStringList fileNames;
    for (int i = 0; i < fileNamesRaw.length() && i < 5; i++) {
        if (fileNamesRaw[i].endsWith(".ts")) {
            if (settings) {
                QString dir = QFileInfo(fileNamesRaw[0]).absoluteDir().absolutePath();
                settings->setValue("directory/video", dir);
            }
            fileNames.append(fileNamesRaw[i]);
        } else if (fileNamesRaw[i].endsWith(".tsc")) {
            if (settings) {
                QString dir = QFileInfo(fileNamesRaw[0]).absoluteDir().absolutePath();
                settings->setValue("directory/video", dir);
            }
            QFile inFile(fileNamesRaw[i]);
            inFile.open(QIODevice::ReadOnly);
            QTextStream in(&inFile);
            QFileInfo fi(fileNamesRaw[i]);
            QDir d = fi.absoluteDir();
            QString line;
            while (in.readLineInto(&line)) {
                if (line.isEmpty()) {
                    fileNames.append("");
                } else {
                    QString full = d.absoluteFilePath(line);
                    full = QDir::cleanPath(full);
                    fileNames.append(full);
                }
            }
        }
    }
    return fileNames;
}
