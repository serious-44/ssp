#include "clipeditor.h"
#include "ui_clipeditor.h"

#include "util.h"

#include <limits.h>
#include <stdlib.h>

#include <QFileDialog>
#include <QFile>
#include <QTextCursor>
#include <QImage>
#include <QImageReader>
#include <QSettings>
#include <QTimer>
#include <QAudioDeviceInfo>

ClipEditor::ClipEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClipEditor),
    inSliderDrag(false),
    inSlotPositionChanged(false),
    active(false),
    mediaPlayer(0),
    currentPieces(4),
    snapWasDisabled(true),
    oldFindString("\x01\x02\x03")
{
    ui->setupUi(this);

    tsHighlihter = new MyHighlighter(this, ui->editTs->document());

    connect(ui->buttonOpenFile, SIGNAL (clicked()), this, SLOT (slotOpenFile()));
    connect(ui->buttonSaveTsFile, SIGNAL (clicked()), this, SLOT (slotSaveTsFile()));
    connect(ui->buttonSavePreview, SIGNAL (clicked()), this, SLOT (slotSavePreview()));
    connect(ui->buttonPrepareImage, SIGNAL (clicked()), this, SLOT (slotPrepareImage()));

    connect(ui->editTs, SIGNAL (textChanged()), this, SLOT (slotTextChanged()));
    connect(ui->editTs, SIGNAL (cursorPositionChanged()), this, SLOT (slotTextCursorChanged()));

    connect(ui->buttonPieces4, SIGNAL (clicked()), this, SLOT (slotPieces4()));
    connect(ui->buttonPieces3, SIGNAL (clicked()), this, SLOT (slotPieces3()));
    connect(ui->buttonPieces2, SIGNAL (clicked()), this, SLOT (slotPieces2()));
    connect(ui->buttonPieces1, SIGNAL (clicked()), this, SLOT (slotPieces1()));
    connect(ui->buttonPieces0, SIGNAL (clicked()), this, SLOT (slotPieces0()));
    connect(ui->buttonIntro, SIGNAL (clicked()), this, SLOT (slotIntro()));
    connect(ui->buttonTake, SIGNAL (clicked()), this, SLOT (slotTake()));
    connect(ui->buttonDrop, SIGNAL (clicked()), this, SLOT (slotDrop()));
    connect(ui->buttonWin, SIGNAL (clicked()), this, SLOT (slotWin()));
    connect(ui->buttonLose, SIGNAL (clicked()), this, SLOT (slotLose()));
    connect(ui->buttonShow, SIGNAL (clicked()), this, SLOT (slotShow()));
    connect(ui->buttonOn4, SIGNAL (clicked()), this, SLOT (slotOn4()));
    connect(ui->buttonOn3, SIGNAL (clicked()), this, SLOT (slotOn3()));
    connect(ui->buttonOn2, SIGNAL (clicked()), this, SLOT (slotOn2()));
    connect(ui->buttonOn1, SIGNAL (clicked()), this, SLOT (slotOn1()));
    connect(ui->buttonOff3, SIGNAL (clicked()), this, SLOT (slotOff3()));
    connect(ui->buttonOff2, SIGNAL (clicked()), this, SLOT (slotOff2()));
    connect(ui->buttonOff1, SIGNAL (clicked()), this, SLOT (slotOff1()));
    connect(ui->buttonOff0, SIGNAL (clicked()), this, SLOT (slotOff0()));
    connect(ui->buttonDead, SIGNAL (clicked()), this, SLOT (slotDead()));
    connect(ui->buttonNo, SIGNAL (clicked()), this, SLOT (slotNo()));
    connect(ui->buttonX, SIGNAL (clicked()), this, SLOT (slotX()));

    connect(ui->buttonModGood, SIGNAL (clicked()), this, SLOT (slotModGood()));
    connect(ui->buttonModBad, SIGNAL (clicked()),  this, SLOT (slotModBad()));
    connect(ui->buttonModThrow, SIGNAL (clicked()), this, SLOT (slotModThrow()));
    connect(ui->buttonModYoulose, SIGNAL (clicked()), this, SLOT (slotModYoulose()));
    connect(ui->buttonModYouwin, SIGNAL (clicked()), this, SLOT (slotModYouwin()));
    connect(ui->buttonModCards, SIGNAL (clicked()), this, SLOT (slotModCards()));
    connect(ui->buttonNoWCards, SIGNAL (clicked()), this, SLOT (slotModNoWCards()));
    connect(ui->buttonModHigh, SIGNAL (clicked()), this, SLOT (slotModHigh()));
    connect(ui->buttonModLow, SIGNAL (clicked()), this, SLOT (slotModLow()));
    connect(ui->buttonModDrink, SIGNAL (clicked()), this, SLOT (slotModDrink()));
    connect(ui->buttonMute, SIGNAL (clicked()), this, SLOT (slotToggleMute()));

    connect(ui->sliderPosition, SIGNAL (valueChanged(int)), this, SLOT (slotSliderMoved(int)));
    connect(ui->sliderPosition, SIGNAL (sliderPressed()), this, SLOT (slotSliderPressed()));
    connect(ui->sliderPosition, SIGNAL (sliderReleased()), this, SLOT (slotSliderReleased()));
    connect(ui->buttonPlay, SIGNAL (clicked()), this, SLOT (slotPlayStop()));
    connect(ui->buttonPosStart, SIGNAL (clicked()), this, SLOT (slotPosStart()));
    connect(ui->buttonPosBack, SIGNAL (clicked()), this, SLOT (slotPosBack()));
    connect(ui->buttonPosBack1, SIGNAL (clicked()), this, SLOT (slotPosBack1()));
    connect(ui->buttonPosForward1, SIGNAL (clicked()), this, SLOT (slotPosForward1()));
    connect(ui->buttonPosForward, SIGNAL (clicked()), this, SLOT (slotPosForward()));
    connect(ui->buttonPosEnd, SIGNAL (clicked()), this, SLOT (slotPosEnd()));
    connect(ui->buttonGotoMark, SIGNAL (clicked()), this, SLOT (slotPosGotoMark()));
    connect(ui->buttonPrevMark, SIGNAL (clicked()), this, SLOT (slotPosPrevMark()));
    connect(ui->buttonNextMark, SIGNAL (clicked()), this, SLOT (slotPosNextMark()));
    connect(ui->buttonSnap, SIGNAL (clicked()), this, SLOT (slotPosSnap()));
    connect(ui->buttonTrimPreview, SIGNAL (clicked(bool)), this, SLOT (slotTrimPreview(bool)));
    connect(ui->buttonTrimImage, SIGNAL (clicked(bool)), this, SLOT (slotTrimImage(bool)));
    connect(ui->buttonTimestamp, SIGNAL (clicked(bool)), this, SLOT (slotTimestamp()));
    connect(ui->buttonScanAudio, SIGNAL (clicked()), this, SLOT (slotScanAudio()));
    connect(ui->buttonFind, SIGNAL (clicked()), this, SLOT (slotFind()));
    connect(ui->editFind, SIGNAL (currentIndexChanged(int)), this, SLOT (slotFind()));
    connect(ui->buttonCheck, SIGNAL (clicked()), this, SLOT (slotCheck()));
    connect(ui->buttonGenerateExtra, SIGNAL (clicked()), this, SLOT (slotGenerateExtra()));

    connect(ui->buttonDownloadPreview, SIGNAL (clicked()), this, SLOT (slotDownloadPreview()));

    mediaPlayer = new QMediaPlayer();
    mediaPlayerEnd = new QMediaPlayer();
    mediaPlayerStart = new QMediaPlayer();
    //currentPosition = 0;
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(slotPositionChanged(qint64)));
    connect(mediaPlayer, SIGNAL(metaDataAvailableChanged(bool)), this, SLOT(slotMetaDataAvailableChanged(bool)));
    connect(mediaPlayer, SIGNAL(metaDataChanged(const QString&, const QVariant&)), this, SLOT(slotMetaDataChanged(const QString &, const QVariant &)));
    connect(mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(slotDurationChanged(qint64)));
    connect(mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(slotMediaPlayerError(QMediaPlayer::Error)));
    connect(mediaPlayerEnd, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(slotMediaPlayerError(QMediaPlayer::Error)));
    connect(mediaPlayerStart, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(slotMediaPlayerError(QMediaPlayer::Error)));

    QAudioFormat desired;
    desired.setChannelCount(1);
    desired.setSampleRate(10000);
    desired.setSampleType(QAudioFormat::SignedInt);
    desired.setCodec("audio/x-raw");
    desired.setSampleSize(16);
    audioDecoder.setAudioFormat(desired);
    connect(&audioDecoder, SIGNAL(bufferReady()), this, SLOT(slotAudioBufferAvailable()));
    connect(&audioDecoder, SIGNAL(error(QAudioDecoder::Error)), this, SLOT(slotAudioError(QAudioDecoder::Error)));
    connect(&audioDecoder, SIGNAL(finished()), this, SLOT(slotAudioFinished()));

    QGraphicsScene *scene = new QGraphicsScene(ui->videoView);
    QGraphicsVideoItem *item = new QGraphicsVideoItem();
    ui->videoView->init(scene, item, &networkImage);
    mediaPlayer->setVideoOutput(item);
    ui->videoViewEnd->setAutoFillBackground(true);
    ui->videoViewStart->setAutoFillBackground(true);
    mediaPlayerEnd->setVideoOutput(ui->videoViewEnd);
    mediaPlayerStart->setVideoOutput(ui->videoViewStart);

    //QAudioProbe p;
    //bool ok = p.setSource(mediaPlayer);

    QSettings settings;
    if (settings.contains("clipeditor/splitter1")) {
        QRect geometry = settings.value("clipeditor/geometry").toRect();
        bool fullScreen = settings.value("clipeditor/fullScreen").toBool();
        setGeometry(geometry);
        if (fullScreen) {
            setWindowState(Qt::WindowFullScreen);
        }
        ui->splitter1->restoreState(settings.value("clipeditor/splitter1").toByteArray());
        ui->splitter2->restoreState(settings.value("clipeditor/splitter2").toByteArray());
    }

}

ClipEditor::~ClipEditor()
{
    if (mediaPlayer) {
        mediaPlayer->stop();
        mediaPlayer->deleteLater();
        mediaPlayer = 0;
        mediaPlayerEnd->stop();
        mediaPlayerEnd->deleteLater();
        mediaPlayerEnd = 0;
        mediaPlayerStart->stop();
        mediaPlayerStart->deleteLater();
        mediaPlayerStart = 0;
    }
    delete ui;
}

void ClipEditor::prepareClose(bool save) {
    if (save) {
        slotSaveTsFile();
    }
    QSettings settings;
    settings.setValue("clipeditor/geometry", geometry());
    settings.setValue("clipeditor/fullScreen", isFullScreen());
    settings.setValue("clipeditor/splitter1", ui->splitter1->saveState());
    settings.setValue("clipeditor/splitter2", ui->splitter2->saveState());
}

bool ClipEditor::needsSave() {
    return ui->buttonSaveTsFile->isEnabled();
}

void ClipEditor::slotOpenFile() {
    if (ui->buttonScanAudio->text() == "Running...") { //FIXME
        audioDecoder.stop();
        ui->buttonScanAudio->setText("Scan audio");
    }

    QSettings settings;
    QString dir;
    if (settings.contains("directory/video")) {
        dir = settings.value("directory/video").toString();
    } else {
        dir = QDir::homePath();
    }

    QFileDialog dialog(this, "Select a video file", dir, "Video (*.avi)");
    dialog.setFileMode(QFileDialog::ExistingFile);

    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
        videoFileName = fileNames[0];
        showInfo(QString("Open video file %1").arg(videoFileName));
        duration = -1;
        mediaPlayer->setMedia(QUrl::fromLocalFile(videoFileName));
        mediaPlayer->play();
        mediaPlayer->pause();
        mediaPlayerEnd->setMedia(QUrl::fromLocalFile(videoFileName));
        mediaPlayerEnd->play();
        mediaPlayerEnd->pause();
        mediaPlayerStart->setMedia(QUrl::fromLocalFile(videoFileName));
        mediaPlayerStart->play();
        mediaPlayerStart->pause();

        tsFileName = videoFileName;
        tsFileName.replace(Util::regPatternVideo, ".ts");
        QFile inFile(tsFileName);
        if (inFile.exists()) {
            if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                showFatalError(QString("Can't open %1: %2").arg(tsFileName).arg(inFile.errorString()));
                return;
            }
            QTextStream in(&inFile);
            QString line;
            QString buffer;
            while (in.readLineInto(&line)) {
                buffer.append(Util::formatTsLine(line));
                buffer.append('\n');
            }
            ui->editTs->setPlainText(buffer);
            QTextCursor tc = ui->editTs->textCursor();
            tc.movePosition(QTextCursor::End);
            ui->editTs->setTextCursor(tc);
            showInfo(QString("Open timestamp file %1").arg(tsFileName));
        } else {
            ui->editTs->setPlainText("00:00:00:000");
            QTextCursor tc = ui->editTs->textCursor();
            tc.movePosition(QTextCursor::End);
            ui->editTs->setTextCursor(tc);
            showInfo(QString("New timestamp file %1").arg(tsFileName));
        }

        dir = QFileInfo(fileNames[0]).absoluteDir().absolutePath();
        settings.setValue("directory/video", dir);

        ui->buttonSaveTsFile->setEnabled(false);

        snap2.clear();

        QString snapFileName = videoFileName;
        snapFileName.replace(Util::regPatternVideo, ".snap2");
        QFile snapFile(snapFileName);
        if (snapFile.exists()) {
            if (!snapFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                showFatalError(QString("Can't open %1: %2").arg(snapFileName).arg(snapFile.errorString()));
            } else {
                snap2.append(0);
                char c;
                bool done = false;
                int motion = 0;
                while (!done && snapFile.getChar(&c)) {
                    if (c >= '0' && c <= '9') {
                        motion *= 10;
                        motion += c-'0';
                    } else if (c == '\n') {
                        snap2.append(motion);
                        motion = 0;
                    } else {
                        done = true;
                    }
                }
                for (int i = 0; i < 100; i++) {
                    snap2.append(0);
                }
                showInfo(QString("Open snap file %1").arg(snapFileName));
            }
        } else {
            showInfo(QString("No snap file found %1").arg(snapFileName));
        }

        ui->checkAutosnap->setEnabled(snap2.size() > 0);
        ui->buttonSnap->setEnabled(snap2.size() > 0);

        active = true;
    }
}

void ClipEditor::slotSaveTsFile() {
    QString backFileName = tsFileName;
    backFileName += ".bak";
    QFile backFile(backFileName);
    if (backFile.exists()) {
        if (!backFile.remove()) {
            showError(QString("Can't remove %1: %2").arg(backFileName).arg(backFile.errorString()));
        }
    }
    QFile oldTsFile(tsFileName);
    if (oldTsFile.exists()) {
        if (!oldTsFile.rename(backFileName)) {
            showError(QString("Can't rename %1 to .bak: %2").arg(tsFileName).arg(oldTsFile.errorString()));
        }
    }
    QFile tsFile(tsFileName);
    if (!tsFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        showFatalError(QString("Can't open %1: %2").arg(tsFileName).arg(tsFile.errorString()));
        return;
    }
    QTextStream out(&tsFile);
    out << ui->editTs->toPlainText();

    showInfo(QString("Save timestamp file %1").arg(tsFileName));

    ui->buttonSaveTsFile->setEnabled(false);
}

void ClipEditor::slotSavePreview() {
    QImage img = ui->videoView->trimedPreview();

    QString previewFileName(videoFileName);
    previewFileName.replace(Util::regPatternVideo, ".jpg");
    QString backFileName(previewFileName);
    backFileName += ".bak";
    QFile backFile(backFileName);
    if (backFile.exists()) {
        if (!backFile.remove()) {
            showError(QString("Can't remove %1: %2").arg(backFileName).arg(backFile.errorString()));
        }
    }
    QFile oldFile(previewFileName);
    if (oldFile.exists()) {
        if (!oldFile.rename(backFileName)) {
            showError(QString("Can't rename %1 to .bak: %2").arg(previewFileName).arg(oldFile.errorString()));
        }
    }
    QFile outFile(previewFileName);
    if (!outFile.open(QIODevice::WriteOnly)) {
        showFatalError(QString("Can't open %1: %2").arg(previewFileName).arg(outFile.errorString()));
        return;
    }
    img.save(&outFile, "JPG");

    showInfo(QString("Save preview file %1").arg(previewFileName));

    ui->buttonTrimPreview->setChecked(false);
    ui->buttonTrimImage->setChecked(false);
    ui->buttonSavePreview->setEnabled(false);
    ui->buttonPrepareImage->setEnabled(false);
    ui->videoView->slotTrimPreview(false);
    ui->videoView->showItem(0);
}

void ClipEditor::slotPrepareImage() {
    QRect trim = ui->videoView->trimedRect();

    QString str;
    if (ui->buttonTrimImage->isChecked()) {
        QString name = ui->textPreviewURL->text();
        str = QString("#preview-url:%1\n#preview-crop:%2,%3,%4,%5\n").arg(name).arg(trim.x()).arg(trim.y()).arg(trim.width()).arg(trim.height());
    } else {
        QString time = Util::timestampToFileTimecode(mediaPlayer->position());
        str = QString("#preview-time:%1\n#preview-crop:%2,%3,%4,%5\n").arg(time).arg(trim.x()).arg(trim.y()).arg(trim.width()).arg(trim.height());
    }

    QTextCursor line = ui->editTs->textCursor();
    line.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    line.insertText(str);
    line.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    ui->editTs->setTextCursor(line);
    ui->editTs->ensureCursorVisible();

    ui->buttonTrimPreview->setChecked(false);
    ui->buttonTrimImage->setChecked(false);
    ui->buttonSavePreview->setEnabled(false);
    ui->buttonPrepareImage->setEnabled(false);
    ui->videoView->slotTrimPreview(false);
    ui->videoView->showItem(0);
}

//void ClipEditor::slotEnableSavePreview() {
//    ui->buttonSavePreview->setEnabled(true);
//}

void ClipEditor::slotTextChanged() {
    //qDebug() << "text changed";
    ui->buttonSaveTsFile->setEnabled(active);
}

void ClipEditor::slotTextCursorChanged() {
    //qDebug() << "cursor changed";
    if (active) {
        QString line = ui->editTs->textCursor().block().text();
        if (ui->checkGotoMark->isChecked()) {
            qint64 time;
            TimestampLine ok = Util::scanTimestampLine(line, &time);
            if ((ok == TimestampLineAction || ok == TimestampLineTimestamp || ok == TimestampLineIllegal) && time != mediaPlayer->position() && ui->editTs->textCursor().selectedText().isEmpty()) {
                slotPosGotoMark();
            }
        }

        if (Util::regTimecode.indexIn(line) == 0) {
            qint64 time = Util::timecodeToTimestamp(Util::regTimecode);
            //qDebug() << "mediaPlayerStart" << Util::timestampToTimecode(time);
            mediaPlayerStart->setPosition(time);
        }
    }
    tsHighlihter->rehighlight();
}

void ClipEditor::keyPressEvent(QKeyEvent *event) {
    //qDebug() << "key" << event->modifiers() << event->key();
    if ((event->modifiers() && ~Qt::KeypadModifier) == Qt::NoModifier) {
        int k = event->key();
        if (k == Qt::Key_Space || k == Qt::Key_F10) {
            slotPlayStop();
            event->accept();
        } else if (k == Qt::Key_4) {
            insertTsText(QString(), 4, QString());
            event->accept();
        } else if (k == Qt::Key_3) {
            insertTsText(QString(), 3, QString());
            event->accept();
        } else if (k == Qt::Key_2) {
            insertTsText(QString(), 2, QString());
            event->accept();
        } else if (k == Qt::Key_1) {
            insertTsText(QString(), 1, QString());
            event->accept();
        } else if (k == Qt::Key_0) {
            insertTsText(QString(), 0, QString());
            event->accept();
        } else if (k == Qt::Key_5) {
            ui->checkZoom->setChecked(!ui->checkZoom->isChecked());
            event->accept();
        } else if (k == Qt::Key_I) {
            insertTsText("intro", 4, QString());
            event->accept();
        } else if (k == Qt::Key_T) {
            insertTsText("take", -1, QString());
            event->accept();
        } else if (k == Qt::Key_G) {
            insertTsText("take", -1, "good");
            event->accept();
        } else if (k == Qt::Key_M) {
            insertTsText("take", -1, "bad");
            event->accept();
        } else if (k == Qt::Key_D) {
            insertTsText("drop", -1, QString());
            event->accept();
        } else if (k == Qt::Key_R) {
            insertTsText("drop", -1, "throw");
            event->accept();
        } else if (k == Qt::Key_U) {
            insertTsText("win", -1, QString());
            event->accept();
        } else if (k == Qt::Key_Q) {
            insertTsText("win", -1, "youlose");
            event->accept();
        } else if (k == Qt::Key_L) {
            insertTsText("lose", -1, QString());
            event->accept();
        } else if (k == Qt::Key_P) {
            insertTsText("lose", -1, "youwin");
            event->accept();
        } else if (k == Qt::Key_S) {
            insertTsText("show", -1, QString());
            event->accept();
        } else if (k == Qt::Key_K) {
            insertTsText("show", -1, "drink");
            event->accept();
        } else if (k == Qt::Key_A) {
            insertTsText("show", -1, "cards");
            event->accept();
        } else if (k == Qt::Key_J) {
            insertTsText("no", -1, "cards");
            event->accept();
        } else if (k == Qt::Key_H) {
            insertTsText("show", -1, "high");
            event->accept();
        } else if (k == Qt::Key_E) {
            insertTsText("show", -1, "low");
            event->accept();
        } else if (k == Qt::Key_O) {
            insertTsText("on", -1, QString());
            event->accept();
        } else if (k == Qt::Key_F) {
            insertTsText("off", -1, QString());
            event->accept();
        } else if (k == Qt::Key_9) {
            insertTsText("broke", 0, QString());
            event->accept();
        } else if (k == Qt::Key_N) {
            insertTsText("no", -1, QString());
            event->accept();
        } else if (k == Qt::Key_Minus) {
            videoStop();
            insertTsText("-", -1, QString());
            event->accept();
        } else if (k == Qt::Key_Home) {
            slotPosStart();
            event->accept();
        } else if (k == Qt::Key_Z || k == Qt::Key_Y || k == Qt::Key_W) {
            slotPosBack();
            event->accept();
        } else if (k == Qt::Key_X) {
            slotPosBack1();
            event->accept();
        } else if (k == Qt::Key_C) {
            slotPosForward1();
            event->accept();
        } else if (k == Qt::Key_V) {
            slotPosForward();
            event->accept();
        } else if (k == Qt::Key_End) {
            slotPosEnd();
            event->accept();
        } else if (k == Qt::Key_B) {
            slotPosSnap();
            event->accept();
        } else if (k == Qt::Key_F9) {
            videoStop();
            slotPosGotoMark();
            event->accept();
        } else if (k == Qt::Key_F7) {
            videoStop();
            slotPosPrevMark();
            event->accept();
        } else if (k == Qt::Key_F8) {
            videoStop();
            slotPosNextMark();
            event->accept();
        } else if (k == Qt::Key_Up) {
            QTextCursor jumpLine = ui->editTs->textCursor();
            if (jumpLine.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor) && jumpLine.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor)) {
                ui->editTs->setTextCursor(jumpLine);
            }
            event->accept();
        } else if (k == Qt::Key_Down) {
            QTextCursor jumpLine = ui->editTs->textCursor();
            if (jumpLine.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor) && jumpLine.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor)) {
                ui->editTs->setTextCursor(jumpLine);
            }
            event->accept();
        } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return ) {
            if(ui->textPreviewURL->hasFocus()) {
                 ui->buttonDownloadPreview->click();
            }
            if (ui->editFind->hasFocus()) {
                ui->buttonFind->click();
            }
            event->accept();
        }
    } else if (event->modifiers() == Qt::ControlModifier) {
        int k = event->key();
        if (k == Qt::Key_O) {
            slotOpenFile();
            event->accept();
        } else if (k == Qt::Key_S) {
            slotSaveTsFile();
            event->accept();
        } else if (k == Qt::Key_A) {
            ui->addTimestamp->setChecked(!ui->addTimestamp->isChecked());
            event->accept();
        } else if (k == Qt::Key_T) {
            slotTimestamp();
            event->accept();
        } else if (k == Qt::Key_B) {
            ui->checkAutosnap->setChecked(!ui->checkAutosnap->isChecked());
            event->accept();
        } else if (k == Qt::Key_N) {
            slotScanAudio();
            event->accept();
        } else if (k == Qt::Key_F) {
            slotFind();
            event->accept();
        } else if (k == Qt::Key_J) {
            ui->checkFind->setChecked(!ui->checkFind->isChecked());
            event->accept();
        } else if (k == Qt::Key_G) {
            slotGenerateExtra();
            event->accept();
        } else if (k == Qt::Key_K) {
            slotCheck();
            event->accept();
        } else if (k == Qt::Key_M) {
            slotToggleMute();
            event->accept();
        } else if (k == Qt::Key_F9) {
            ui->checkGotoMark->setChecked(!ui->checkGotoMark->isChecked());
            event->accept();
        }
    } else if (event->modifiers() == Qt::AltModifier) {
        int k = event->key();
        if (k == Qt::Key_T) {
            ui->buttonTrimPreview->setChecked(!ui->buttonTrimPreview->isChecked());
            ui->videoView->slotTrimPreview(ui->buttonTrimPreview->isChecked());
            event->accept();
        } else if (k == Qt::Key_S) {
            slotSavePreview();
            event->accept();
        } else if (k == Qt::Key_P) {
            slotPrepareImage();
            event->accept();
        }
    }
}

void ClipEditor::wheelEvent(QWheelEvent *event) {
    //qDebug() << "wheel" << event->angleDelta() << mediaPlayer->state();
    if (!active) return;
    if (event->angleDelta().y() < 0) {
        mediaPlayer->setPosition(Util::addFramesToTimestamp(mediaPlayer->position(), -25, duration));
        if (mediaPlayer->state() != QMediaPlayer::PlayingState) {
            slotPositionChanged(-1);
        }
    }
    if (event->angleDelta().y() > 0) {
        mediaPlayer->setPosition(Util::addFramesToTimestamp(mediaPlayer->position(), 25, duration));
        if (mediaPlayer->state() != QMediaPlayer::PlayingState) {
            slotPositionChanged(-1);
        }
    }
}

void ClipEditor::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::MidButton) {
        videoStop();
        slotPosGotoMark();
    }
}

void ClipEditor::slotPieces4() {
    insertTsText(QString(), 4, QString());
}
void ClipEditor::slotPieces3() {
    insertTsText(QString(), 3, QString());
}
void ClipEditor::slotPieces2() {
    insertTsText(QString(), 2, QString());
}
void ClipEditor::slotPieces1() {
    insertTsText(QString(), 1, QString());
}
void ClipEditor::slotPieces0() {
    insertTsText(QString(), 0, QString());
}
void ClipEditor::slotIntro() {
    insertTsText("intro", 4, QString());
}
void ClipEditor::slotTake() {
    insertTsText("take", -1, QString());
}
void ClipEditor::slotDrop() {
    insertTsText("drop", -1, QString());
}
void ClipEditor::slotWin() {
    insertTsText("win", -1, QString());
}
void ClipEditor::slotLose() {
    insertTsText("lose", -1, QString());
}
void ClipEditor::slotShow() {
    insertTsText("show", -1, QString());
}
void ClipEditor::slotOn4() {
    insertTsText("on", 4, QString());
}
void ClipEditor::slotOn3() {
    insertTsText("on", 3, QString());
}
void ClipEditor::slotOn2() {
    insertTsText("on", 2, QString());
}
void ClipEditor::slotOn1() {
    insertTsText("on", 1, QString());
}
void ClipEditor::slotOff3() {
    insertTsText("off", 3, QString());
}
void ClipEditor::slotOff2() {
    insertTsText("off", 2, QString());
}
void ClipEditor::slotOff1() {
    insertTsText("off", 1, QString());
}
void ClipEditor::slotOff0() {
    insertTsText("off", 0, QString());
}
void ClipEditor::slotDead() {
    insertTsText("broke", 0, QString());
}
void ClipEditor::slotNo() {
    insertTsText("no", -1, QString());
}
void ClipEditor::slotX() {
    insertTsText("-", -1, QString());
}

void ClipEditor::slotModGood() {
    insertTsText("take", -1, "good");
}

void ClipEditor::slotModBad() {
    insertTsText("take", -1, "bad");
}

void ClipEditor::slotModThrow() {
    insertTsText("drop", -1, "throw");
}

void ClipEditor::slotModYoulose() {
    insertTsText("win", -1, "youlose");
}

void ClipEditor::slotModYouwin() {
    insertTsText("lose", -1, "youwin");
}

void ClipEditor::slotModCards() {
    insertTsText("show", -1, "cards");
}

void ClipEditor::slotModNoWCards() {
    insertTsText("no", -1, "cards");
}

void ClipEditor::slotModHigh() {
    insertTsText("show", -1, "high");
}

void ClipEditor::slotModLow() {
    insertTsText("show", -1, "low");
}

void ClipEditor::slotModDrink() {
    insertTsText("show", -1, "drink");
}

void ClipEditor::insertTsText(QString action, int pieces, QString modifier) {
    bool hasZoom = action != "-" && action != "on" && action != "off" && action != "intro" && action != "broke";
    insertTsText(action, pieces, modifier, hasZoom ? ui->checkZoom->isChecked() : false);
}
void ClipEditor::insertTsText(QString action, int pieces, QString modifier, bool zoom) {
    if (!active) return;

    if (pieces < 0) {
        pieces = currentPieces;
    }

    if (!action.isEmpty()) {
        videoStop();

        bool jumpToNext = false;

        QString oldText;
        //QString oldModifier;
        QTextCursor replace;

        QTextCursor line = ui->editTs->textCursor();
        line.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        if (line.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor)) {
            oldText = line.selectedText();
        }

        //if (regActionModifier.indexIn(oldText) == 0) {
        //    oldModifier = QString(" ") + regActionModifier.cap(1);
        //}

        //QString newText = QString(" %1 %2%3").arg(action).arg(pieces).arg(oldModifier);

        QString newText = QString(" %1 %2 %3 %4").arg(action, -5).arg(pieces).arg(modifier, -7).arg(zoom ? " zoom" : "     ");

        if (Util::regTimecode.indexIn(oldText) == 0) {
            replace = ui->editTs->textCursor();
            replace.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            for (int i = 0; i < (2+1+2+1+2+1+3); i++) {
                replace.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor);
            }
            replace.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        }
        if (ui->addTimestamp->isChecked()) {
            QTextCursor nextLine = ui->editTs->textCursor();
            if (nextLine.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor) && nextLine.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor)) {
                QString nextText = nextLine.selectedText();
                if (Util::regTimecode.indexIn(nextText) == 0) {
                    qint64 time = Util::timecodeToTimestamp(Util::regTimecode);

                    time -= mediaPlayer->position();
                    if (time < -1000 || time > 1000) {
                        newText += "\n";
                        newText += Util::timestampToFileTimecode(mediaPlayer->position());
                    } else {
                        jumpToNext = true;
                    }
                }
            } else {
                newText += "\n";
                newText += Util::timestampToFileTimecode(mediaPlayer->position());
            }
        }
        replace.insertText(newText);
        if (jumpToNext) {
            QTextCursor jumpLine = ui->editTs->textCursor();
            if (jumpLine.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor) && jumpLine.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor)) {
                ui->editTs->setTextCursor(jumpLine);
                slotPosGotoMark();
            }
        } else {
            replace.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
            ui->editTs->setTextCursor(replace);
        }
        ui->editTs->ensureCursorVisible();

        if (ui->checkFind->isChecked()) {
            slotFind();
        }
    }

    if (currentPieces != pieces) {
        currentPieces = pieces;
        if (currentPieces == 4) ui->buttonPieces4->setChecked(true);
        if (currentPieces == 3) ui->buttonPieces3->setChecked(true);
        if (currentPieces == 2) ui->buttonPieces2->setChecked(true);
        if (currentPieces == 1) ui->buttonPieces1->setChecked(true);
        if (currentPieces == 0) ui->buttonPieces0->setChecked(true);
    }
}

void ClipEditor::slotToggleMute() {
    if (!active) return;

    QTextCursor c = ui->editTs->textCursor();
    QTextBlock line = c.block();

    qint64 time;
    QString action;
    int pieces;
    QString modifier;
    bool zoom;
    ClipSound quiet;
    if (Util::scanTimestampLine(line.text(), &time, &action, &pieces, &modifier, &zoom, &quiet) == TimestampLineAction) {
        if (quiet == ClipSoundMute) {
            quiet = ClipSoundLoud;
        } else {
            quiet = ClipSoundMute;
        }
        QString nl = Util::formatTsLine(time, action, pieces, modifier, zoom, quiet);
        c.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        c.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        c.insertText(nl);
        c.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
        ui->editTs->setTextCursor(c);
    }

    ui->editTs->ensureCursorVisible();

    if (ui->checkFind->isChecked()) {
        slotFind();
    }
}

void ClipEditor::slotTimestamp() {
    if (!active) return;

    QTextCursor line = ui->editTs->textCursor();
    line.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    line.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    QString oldText = line.selectedText();
    line.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    if (Util::regRoughTimecode.indexIn(oldText) == 0) {
        for (int i = 0; i < Util::regRoughTimecode.cap(1).length(); i++) {
            line.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        }
    }
    QString newText = Util::timestampToFileTimecode(mediaPlayer->position()) + " ";
    line.insertText(newText);
    line.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
    ui->editTs->setTextCursor(line);
    ui->editTs->ensureCursorVisible();
}

void ClipEditor::slotPlayStop() {
    if (!active) return;

    if (mediaPlayer->state() == QMediaPlayer::PlayingState) {
        mediaPlayer->pause();
        slotPositionChanged(-1);
        if (ui->checkAutosnap->isEnabled() && ui->checkAutosnap->isChecked()) {
            slotPosSnap();
        }
    } else {
        mediaPlayer->play();
    }
}

void ClipEditor::videoStop() {
    if (!active) return;

    if (mediaPlayer->state() == QMediaPlayer::PlayingState) {
        mediaPlayer->pause();
        slotPositionChanged(-1);
    }
}

void ClipEditor::slotSliderMoved(int value) {
    if (!active) return;
    if (inSlotPositionChanged) return;

    //qDebug() << "sliderMoved" << value << duration << "=" << ((qint64)value * duration + 500) / 1000;
    //videoStop();
    if (duration > 0) {
        mediaPlayer->setPosition(((qint64)value * duration + 500) / 1000);
        slotPositionChanged(-1);
    }
}

void ClipEditor::slotSliderPressed() {
    //inSliderDrag = true;
}

void ClipEditor::slotSliderReleased() {
    //inSliderDrag = false;
}

void ClipEditor::slotPosStart() {
    if (!active) return;
    videoStop();
    mediaPlayer->setPosition(0);
    slotPositionChanged(-1);
}

void ClipEditor::slotPosBack() {
    if (!active) return;
    videoStop();
    mediaPlayer->setPosition(Util::addFramesToTimestamp(mediaPlayer->position(), -10, duration));
    slotPositionChanged(-1);
}

void ClipEditor::slotPosBack1() {
    if (!active) return;
    videoStop();
    mediaPlayer->setPosition(Util::addFramesToTimestamp(mediaPlayer->position(), -1, duration));
    slotPositionChanged(-1);
}

void ClipEditor::slotPosForward1() {
    if (!active) return;
    videoStop();
    mediaPlayer->setPosition(Util::addFramesToTimestamp(mediaPlayer->position(), 1, duration));
    slotPositionChanged(-1);
}

void ClipEditor::slotPosForward() {
    if (!active) return;
    videoStop();
    mediaPlayer->setPosition(Util::addFramesToTimestamp(mediaPlayer->position(), 10, duration));
    slotPositionChanged(-1);
}

void ClipEditor::slotPosEnd() {
    if (!active) return;
    videoStop();
    if (duration > 0) {
        mediaPlayer->setPosition(duration - 1);
        //qDebug() << "setPosition" << (duration - 1);
        slotPositionChanged(-1);
    }
}

void ClipEditor::slotPosSnap() {
    if (!active) return;
    videoStop();
    qint64 t = mediaPlayer->position();

    int tf = t * 25 / 1000;
    if (snap2.size() > tf + 10) {
        qint64 bestT = tf;
        uint bestVal = 0;
        //qDebug() << "snap" << t << tf;
        for (qint64 tt = tf > 22 ? tf - 22 : 0; tt <= tf; tt++) {
            //qDebug() << tt << snap2[tt];
            if (snap2[tt] > bestVal) {
                bestT = tt;
                bestVal = snap2[tt];
            }
        }
        qint64 nt = bestT * 1000 / 25;
        //qDebug() << "snap to" << nt << bestT;
        mediaPlayer->setPosition(nt);
        slotPositionChanged(-1);
    }
}

void ClipEditor::slotPosGotoMark() {
    if (!active) return;

    QTextCursor c = ui->editTs->textCursor();
    c.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    if (c.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor)) {
        QString line = c.selectedText();
        //qDebug() << line;
        if (Util::regTimecode.indexIn(line) == 0) {
            qint64 time = Util::timecodeToTimestamp(Util::regTimecode);
            //FIXME check valid position
            mediaPlayer->setPosition(time);
            slotPositionChanged(-1);
        }
    }
}

void ClipEditor::slotPosPrevMark() {
    if (!active) return;

    QTextCursor c = ui->editTs->textCursor();
    c.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor);
    if (c.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor)) {
        QString line = c.selectedText();
        //qDebug() << line;
        if (Util::regTimecode.indexIn(line) == 0) {
            qint64 time = Util::timecodeToTimestamp(Util::regTimecode);
            //FIXME check valid position
            mediaPlayer->setPosition(time);
            c.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
            ui->editTs->setTextCursor(c);
            slotPositionChanged(-1);
        }
    }
}

void ClipEditor::slotPosNextMark() {
    if (!active) return;

    QTextCursor c = ui->editTs->textCursor();
    c.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);
    if (c.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor)) {
        QString line = c.selectedText();
        //qDebug() << line;
        if (Util::regTimecode.indexIn(line) == 0) {
            qint64 time = Util::timecodeToTimestamp(Util::regTimecode);
            //FIXME check valid position
            mediaPlayer->setPosition(time);
            c.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
            ui->editTs->setTextCursor(c);
            slotPositionChanged(-1);
        }
    }
}

void ClipEditor::slotFind() {
    if (!active) return;

    videoStop();

    QTextDocument *document = ui->editTs->document();

    if (ui->editFind->lineEdit()->text() != oldFindString) {
        findExpr = QRegExp(ui->editFind->lineEdit()->text());
        if (!findExpr.isValid()) {
            ui->editFind->setStyleSheet("background:#ff8080");
            return;
        }
        ui->editFind->setStyleSheet("");
        oldFindString = ui->editFind->lineEdit()->text();
        //findCursor = QTextCursor(document);
    }
    findCursor = ui->editTs->textCursor();
    findCursor.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
    //if (findCursor.isNull()) {
    //    findCursor = QTextCursor(document);
    //}
    if (!findCursor.isNull() /* && !findCursor.atEnd()*/) {
        findCursor = document->find(findExpr, findCursor);
        if (findCursor.isNull()) {
            findCursor = QTextCursor(document);
            findCursor = document->find(findExpr, findCursor);
            if (findCursor.isNull()) {
                ui->buttonFind->setStyleSheet("color:#801010");
            } else {
                ui->buttonFind->setStyleSheet("");
                ui->editTs->setTextCursor(findCursor);
                slotPosGotoMark();
            }
        } else {
            ui->buttonFind->setStyleSheet("");
            ui->editTs->setTextCursor(findCursor);
            slotPosGotoMark();
        }
    }
    ui->editTs->ensureCursorVisible();
    tsHighlihter->rehighlight();
}

void ClipEditor::slotPositionChanged(qint64 t) {
    inSlotPositionChanged = true;
    bool calledByVideoPlayer = t >= 0;
    //bool foundSnap = false;

    if (!calledByVideoPlayer) {
        if (active) {
            t = mediaPlayer->position();
        } else {
            t = 0;
        }
    }

    QString tc = Util::timestampToTimecode(t);
    ui->labelPosition->setText(tc);
    if (duration <= 0) {
        //qDebug() << "set slider" << 0;
        ui->sliderPosition->setValue(0);
    } else {
        //qDebug() << "set slider" << ((t * 1000 + 500) / duration);
        ui->sliderPosition->setValue((t * 1000 + 500) / duration);
    }

    tsHighlihter->rehighlight();

    if (!calledByVideoPlayer) {
        qint64 end = t >= (1000/25) ? t - (1000/25) : 0;
        //qDebug() << "mediaPlayerEnd" << Util::timestampToTimecode(end);
        mediaPlayerEnd->setPosition(end);
    }

    inSlotPositionChanged = false;
}

void ClipEditor::slotMetaDataAvailableChanged(bool available) {
    //qDebug() << "slotMetaDataAvsilableChanged" << available;
    if (available) {
        duration = mediaPlayer->metaData("Duration").toLongLong();
        ui->videoView->videoChanged();
    }
}

void ClipEditor::slotMetaDataChanged(const QString &key, const QVariant &/*value*/) {
    if (key == "Resolution") {
        //qDebug() << "slotMetaDataChanged" << key << value;
        ui->videoView->videoChanged();
    }
}

void ClipEditor::slotDurationChanged(qint64 dur) {
    //qDebug() << "slotDurationChanged" << dur;
    duration = dur;
}

void ClipEditor::slotMediaPlayerError(QMediaPlayer::Error err) {
    showFatalError(QString("Error in Mediaplayer: %1").arg(mediaPlayer->errorString()));
}


MyHighlighter::MyHighlighter(ClipEditor *e, QTextDocument *parent) :
    QSyntaxHighlighter(parent),
    editor(e)
{

}

MyHighlighter::~MyHighlighter() {

}

void MyHighlighter::highlightBlock(const QString &text) {
    QTextCharFormat errorFormat;
    QTextCharFormat timestampFormat;
    bool hasError = false;

    if (editor->mediaPlayer) {
        qint64 time;
        QString action;
        TimestampLine valid = Util::scanTimestampLine(text, &time, &action);
        if (valid == TimestampLineTimestamp || valid == TimestampLineAction || valid == TimestampLineIllegal) {
            QTextBlock line = editor->ui->editTs->textCursor().block();
            if ( line == currentBlock()) {
                //timestampFormat.setFontUnderline(true);
                timestampFormat.setFontStrikeOut(true);
            }

            qint64 t = time - editor->mediaPlayer->position();
            if (t < 0) t = -t;
            if (t == 0) {
                timestampFormat.setBackground(QBrush(QColor(255,180,0)));
            } else if (t < 2000) {
                int b = t / 20;
                timestampFormat.setBackground(QBrush(QColor(155+b, 155+b, 155+b)));
            }
        }
        if (valid == TimestampLineComment) {
            errorFormat.setForeground(QBrush(QColor(100,100,100)));
            timestampFormat.setForeground(QBrush(QColor(100,100,100)));
        }
        if (valid == TimestampLineAction) {
            QTextBlock current = currentBlock();
            QTextBlock previous = current.previous();
            if (!hasError && previous.isValid()) {
                qint64 prevTime;
                QString prevAction;
                TimestampLine prevValid = Util::scanTimestampLine(previous.text(), &prevTime, &prevAction);
                if ((prevValid == TimestampLineTimestamp || prevValid == TimestampLineAction || prevValid == TimestampLineIllegal)) {
                    qint64 minDiff = 500;
                    if (prevAction.length() < 2) {
                        minDiff = LLONG_MIN;
                    }
                    if (time - prevTime < minDiff) {
                        hasError = true;
                    }
                }
            }
            QTextBlock next = current.next();
            if (!hasError && next.isValid()) {
                qint64 nextTime;
                QString nextAction;
                TimestampLine nextValid = Util::scanTimestampLine(next.text(), &nextTime, &nextAction);
                if (nextValid == TimestampLineTimestamp || nextValid == TimestampLineAction || nextValid == TimestampLineIllegal) {
                    qint64 minDiff = 500;
                    if (action.length() < 2) {
                        minDiff = LLONG_MIN;
                    }
                    if (nextTime - time < minDiff) {
                        hasError = true;
                    }
                }
            }
        }
        if (valid != TimestampLineComment && valid != TimestampLineAction) {
            hasError = true;
        }
    }
    if (hasError) {
        errorFormat.setForeground(QBrush(QColor(150,0,0)));
        timestampFormat.setForeground(QBrush(QColor(150,0,0)));
    }
    setFormat(0, text.length(), errorFormat); // used for comments
    setFormat(0, 2+1+2+1+2+1+3, timestampFormat);

    if (editor->findExpr.isValid() && !editor->findExpr.isEmpty()) {
        int pos = editor->findExpr.indexIn(text);
        if (pos >= 0 && editor->findExpr.cap().length() > 0) {
            errorFormat.setBackground(QBrush(QColor(255,255,10)));
            setFormat(pos, editor->findExpr.cap().length(), errorFormat);
        }
    }
}

void ClipEditor::slotScanAudio() {
    if (!active) return;

    if (ui->buttonScanAudio->text() == "Running...") {
        audioDecoder.stop();
        ui->buttonScanAudio->setText("Scan audio");
        ui->editTs->setEnabled(true);
        return;
    }
    audioTextBlock.clear();
    audioActiveTextBlock.clear();

    QTextDocument *document = ui->editTs->document();
    QTextBlock line = document->firstBlock();

    qint64 time;
    QString action;
    int pieces;
    QString modifier;
    bool zoom;
    ClipSound quiet;
    while (line.isValid() && Util::scanTimestampLine(line.text(), &time, &action, &pieces, &modifier, &zoom, &quiet) != TimestampLineAction) {
        line = line.next();
    }
    if (line.isValid()) {
        QTextBlock next = line.next();
        while (next.isValid()) {
            qint64 nextTime;
            QString nextAction;
            int nextPieces;
            QString nextModifier;
            bool nextZoom;
            ClipSound nextQuiet;
            while(next.isValid() && Util::scanTimestampLine(next.text(), &nextTime, &nextAction, &nextPieces, &nextModifier, &nextZoom, &nextQuiet) != TimestampLineAction) {
                next = next.next();
            }
            if (next.isValid()) {
                if (action != "-") {
                    int pos = audioTextBlock.length();
                    while (pos > 0 && audioTextBlock[pos-1].start > 10 * time) {
                        pos--;
                    }
                    ScanAudio a(10 * time, 10 * nextTime, line);
                    audioTextBlock.insert(pos, a);
                }
                time = nextTime;
                action = nextAction;
                pieces = nextPieces;
                modifier = nextModifier;
                zoom = nextZoom;
                quiet = nextQuiet;
                line = next;
                next = line.next();
            }
        }
    }

    if (!audioTextBlock.isEmpty()) {
        audioCurrentFrame = 0,
        audioDecoder.setSourceFilename(videoFileName);
        audioDecoder.start();
        ui->editTs->setEnabled(false);
        ui->buttonScanAudio->setText("Running...");
    }
}

void ClipEditor::slotAudioBufferAvailable() {
    //qDebug() << "slotAudioBufferAvailable";
    while(audioDecoder.bufferAvailable()) {
        QAudioBuffer buf = audioDecoder.read();
        //QAudioFormat format = buf.format();
        //byteOrder little endian,
        // channels 1
        // sampleRate 44100 -> 10000
        // sampleSize 16
        // sampleType signedInt

        const qint16 *data = buf.constData<qint16>();
        int frames = buf.frameCount();
        for (int i = 0; i < frames; i++) {
            while(!audioTextBlock.isEmpty() && audioCurrentFrame > audioTextBlock[0].start) {
                audioActiveTextBlock.append(audioTextBlock[0]);
                audioTextBlock.removeFirst();
            }
            for (int i = 0; i < audioActiveTextBlock.length(); i++) {
                if (audioCurrentFrame < audioActiveTextBlock[i].end) {
                    audioActiveTextBlock[i].audioSamples++;
                    if (data[i] > 100) {
                        audioActiveTextBlock[i].audioLoudSamples += data[i];
                    }
                } else {
                    //qDebug() << audioActiveTextBlock[i].line.text() << Util::timestampToTimecode(audioActiveTextBlock[i].start/10) << audioActiveTextBlock[i].audioSamples << (audioActiveTextBlock[i].audioLoudSamples/audioActiveTextBlock[i].audioSamples);
                    qint64 time;
                    QString action;
                    int pieces;
                    QString modifier;
                    bool zoom;
                    ClipSound quiet;
                    Util::scanTimestampLine(audioActiveTextBlock[i].line.text(), &time, &action, &pieces, &modifier, &zoom, &quiet);
                    if (quiet != ClipSoundMute) {
                        bool q = audioActiveTextBlock[i].audioSamples == 0 ? false : (audioActiveTextBlock[i].audioLoudSamples/audioActiveTextBlock[i].audioSamples) < 15;
                        if (action == "throw") {
                            q = true;
                        }
                        if (action == "on") {
                            q = false;
                        }
                        if (action == "off") {
                            q = false;
                        }
                        if (action == "intro") {
                            q = false;
                        }
                        if (action == "broke") {
                            q = false;
                        }
                        if (action == "-") {
                            q = false;
                        }
                        quiet = q ? ClipSoundQuiet : ClipSoundLoud;
                    }
                    QString nl = Util::formatTsLine(time, action, pieces, modifier, zoom, quiet);
                    QTextCursor c(audioActiveTextBlock[i].line);
                    c.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
                    c.insertText(nl);

                    audioActiveTextBlock.remove(i);
                }
            }
            if (audioTextBlock.isEmpty() && audioActiveTextBlock.isEmpty()) {
                //qDebug() << "1";
                audioDecoder.stop();
                //qDebug() << "2";
                ui->editTs->setEnabled(true);
                ui->buttonScanAudio->setText("Scan audio");
                //qDebug() << "3";
            }
            audioCurrentFrame++;
        }
    }
}

void ClipEditor::slotAudioError(QAudioDecoder::Error error) {
    showError(QString("Error in Audio %1").arg(audioDecoder.errorString()));
    //qDebug() << "slotAudioError";
    // Clip without Audio?
    for (int i = 0; i < audioTextBlock.length(); i++) {
        qint64 time;
        QString action;
        int pieces;
        QString modifier;
        bool zoom;
        ClipSound quiet;
        Util::scanTimestampLine(audioTextBlock[i].line.text(), &time, &action, &pieces, &modifier, &zoom, &quiet);
        if (quiet != ClipSoundMute) {
            bool q = action != "-" && action != "on" && action != "off";
            quiet = q ? ClipSoundQuiet : ClipSoundLoud;
        }
        QString nl = Util::formatTsLine(time, action, pieces, modifier, zoom, quiet);
        QTextCursor c(audioTextBlock[i].line);
        c.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        c.insertText(Util::formatTsLine(nl));
    }
    //qDebug() << "e1";
    audioDecoder.stop();
    //qDebug() << "e2";
    ui->editTs->setEnabled(true);
    ui->buttonScanAudio->setText("Scan audio");
    //qDebug() << "e3";
}

void ClipEditor::slotAudioFinished() {
    //qDebug() << "slotAudioFinished" << audioCurrentFrame;
    ui->editTs->setEnabled(true);
    ui->buttonScanAudio->setText("Scan audio");
}


void ClipEditor::slotDownloadPreview() {
    ui->buttonDownloadPreview->setEnabled(false);
    ui->buttonDownloadPreview->setText("0%");

    QNetworkRequest request;
    request.setUrl(ui->textPreviewURL->text());
    networkReply = networkAccessManager.get(request);
    connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotNetworkError(QNetworkReply::NetworkError)));
    connect(networkReply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(slotNetworkErrer(QList<QSslError>)));
    connect(networkReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(slotNetworkUpdateProgress(qint64, qint64)));
    connect(networkReply, SIGNAL(finished()), this, SLOT(slotNetworkFinished()));
}

void ClipEditor::slotNetworkError(QNetworkReply::NetworkError err) {
    if (networkReply) {
        showError(QString("Network error: %1").arg(networkReply->errorString()));
    } else {
        showError(QString("Network error"));
    }
    ui->buttonDownloadPreview->setEnabled(true);
    ui->buttonDownloadPreview->setText("Fetch");
    networkReply->deleteLater();
}

void ClipEditor::slotNetworkError(QList<QSslError> err) {
    if (networkReply) {
        showError(QString("Network error: %1").arg(networkReply->errorString()));
    } else {
        showError(QString("Network error"));
    }
}

void ClipEditor::slotNetworkUpdateProgress(qint64 read, qint64 total) {
    ui->buttonDownloadPreview->setText(QString("%1\%").arg((int)(read*100/total)));
}

void ClipEditor::slotNetworkFinished() {
    ui->buttonDownloadPreview->setEnabled(true);
    ui->buttonDownloadPreview->setText("Fetch");

    //QByteArray b = networkReply->readAll();
    QImageReader r(networkReply);
    QPixmap p = QPixmap::fromImageReader(&r);
    networkImage.setPixmap(p);
    ui->buttonTrimImage->setEnabled(true);
    slotTrimImage(true);
    networkReply->deleteLater();

}

void ClipEditor::slotTrimPreview(bool on) {
    if (on) {
        ui->buttonTrimPreview->setChecked(true);
        ui->buttonTrimImage->setChecked(false);
        ui->buttonSavePreview->setEnabled(true);
        ui->buttonPrepareImage->setEnabled(true);
        ui->videoView->showItem(0);
    } else {
        ui->buttonTrimPreview->setChecked(false);
        ui->buttonTrimImage->setChecked(false);
        ui->buttonSavePreview->setEnabled(false);
        ui->buttonPrepareImage->setEnabled(false);
    }
    ui->videoView->slotTrimPreview(on);
}

void ClipEditor::slotTrimImage(bool on) {
    if (on) {
        ui->buttonTrimImage->setChecked(true);
        ui->buttonTrimPreview->setChecked(false);
        ui->buttonPrepareImage->setEnabled(true);
        ui->buttonSavePreview->setEnabled(true);
        ui->videoView->showItem(1);
    } else {
        ui->buttonTrimImage->setChecked(false);
        ui->buttonTrimPreview->setChecked(false);
        ui->buttonPrepareImage->setEnabled(false);
        ui->buttonSavePreview->setEnabled(false);
        ui->videoView->showItem(0);
    }
    ui->videoView->slotTrimPreview(on);
}

void ClipEditor::showFatalError(QString txt) {
    ui->logBrowser->append("<b><span style=\"color:#ff0000;\">Fatal: "+txt+"</span></b>");
    ui->tabWidget->setCurrentIndex(1);
}

void ClipEditor::showError(QString txt) {
    ui->logBrowser->append("<span style=\"color:#c02020;\">"+txt+"</span>");
    ui->tabWidget->setCurrentIndex(1);
}

void ClipEditor::showInfo(QString txt) {
    ui->logBrowser->append("<span style=\"color:#000000;\">"+txt+"</span>");
}

void ClipEditor::slotCheck() {
    showInfo("");
    ui->tabWidget->setCurrentIndex(1);

    QTextDocument *document = ui->editTs->document();
    QTextBlock line = document->firstBlock();

    int intro[2][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    int take[2][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    int take_good[2][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    int take_bad[2][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    int drop[2][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    int drop_throw[2][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    int win[2][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    int win_youlose[2][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    int lose[2][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    int lose_youwin[2][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    int show[2][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    int show_cards[2][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    int show_drink[2][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    int on[2][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    int off[2][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    int broke[2][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    bool has1  = false;

    qint64 time;
    QString action;
    int pieces;
    QString modifier;
    bool zoom;
    ClipSound quiet;
    while (line.isValid() && Util::scanTimestampLine(line.text(), &time, &action, &pieces, &modifier, &zoom, &quiet) != TimestampLineAction) {
        line = line.next();
    }
    if (line.isValid()) {
        QTextBlock next = line.next();
        while (next.isValid()) {
            qint64 nextTime;
            QString nextAction;
            int nextPieces;
            QString nextModifier;
            bool nextZoom;
            ClipSound nextQuiet;
            while(next.isValid() && Util::scanTimestampLine(next.text(), &nextTime, &nextAction, &nextPieces, &nextModifier, &nextZoom, &nextQuiet) != TimestampLineAction) {
                next = next.next();
            }
            if (next.isValid()) {
                if (pieces == 1 && action != "-") {
                    has1 = true;
                }
                if (action == "take") {
                    if (modifier == "good") {
                        take_good[zoom ? 1 : 0][pieces]++;
                    } else if (modifier == "bad") {
                        take_bad[zoom ? 1 : 0][pieces]++;
                    } else {
                        take[zoom ? 1 : 0][pieces]++;
                    }
                } else if (action == "drop") {
                    if (modifier == "throw") {
                        drop_throw[zoom ? 1 : 0][pieces]++;
                    } else {
                        drop[zoom ? 1 : 0][pieces]++;
                    }
                } else if (action == "win") {
                    if (modifier == "youlose") {
                        win_youlose[zoom ? 1 : 0][pieces]++;
                    } else {
                        win[zoom ? 1 : 0][pieces]++;
                    }
                } else if (action == "lose") {
                    if (modifier == "youwin") {
                        lose_youwin[zoom ? 1 : 0][pieces]++;
                    } else {
                        lose[zoom ? 1 : 0][pieces]++;
                    }
                } else if (action == "show") {
                    if (modifier == "drink") {
                        show_drink[zoom ? 1 : 0][pieces]++;
                    } else if (modifier == "") {
                        show[zoom ? 1 : 0][pieces]++;
                    } else {
                        show_cards[zoom ? 1 : 0][pieces]++;
                    }
                } else if (action == "on") {
                    on[zoom ? 1 : 0][pieces]++;
                } else if (action == "off") {
                    off[zoom ? 1 : 0][pieces]++;
                } else if (action == "intro") {
                    intro[zoom ? 1 : 0][pieces]++;
                } else if (action == "broke") {
                    broke[zoom ? 1 : 0][pieces]++;
                }
                time = nextTime;
                action = nextAction;
                pieces = nextPieces;
                modifier = nextModifier;
                zoom = nextZoom;
                quiet = nextQuiet;
                line = next;
                next = line.next();
            }
        }
    }

    if (intro[0][4] + intro[1][4] == 0) { //FIXME intro[1][4] not allowed
        showError("missing intro 4");
    }
    for (int i = 3; i >= 0; i--) {
        if (off[0][i] + off[1][i] == 0) {
            if (i == 1 && ! has1) {
                showInfo(QString("missing off %1").arg(i));
            } else  {
                showError(QString("missing off %1").arg(i));
            }
        }
    }
    for (int i = 4; i >= 1; i--) {
        if (on[0][i] + on[1][i] == 0) {
            if (i == 1 && ! has1) {
                showInfo(QString("missing on %1").arg(i));
            } else  {
                showError(QString("missing on %1").arg(i));
            }
        }
    }
    if (broke[0][0] + broke[1][0] == 0) {
        showError("missing broke 0");
    }
    for (int i = 4; i >= 0; i--) {
        if (take[0][i] == 0 && (take_good[0][i] == 0 || take_bad[0][i] == 0)) {
            if (i == 1 && ! has1) {
                showInfo(QString("missing %1 take no-zoom").arg(i));
            } else  {
                showError(QString("missing %1 take no-zoom").arg(i));
            }
        }
        if (take[1][i] == 0 && (take_good[1][i] == 0 || take_bad[1][i] == 0)) {
            if (i == 1 && ! has1) {
                showInfo(QString("missing %1 take zoom").arg(i));
            } else  {
                showError(QString("missing %1 take zoom").arg(i));
            }
        }
        if (drop[0][i] == 0 && (drop_throw[0][i] == 0 || lose[0][i] == 0)) {
            if (i == 1 && ! has1) {
                showInfo(QString("missing %1 (drop or (throw and lose)) no-zoom").arg(i));
            } else  {
                showError(QString("missing %1 (drop or (throw and lose)) no-zoom").arg(i));
            }
        }
        if (drop[1][i] == 0 && (drop_throw[1][i] == 0 || lose[1][i] == 0)) {
            if (i == 1 && ! has1) {
                showInfo(QString("missing %1 (drop or (throw and lose)) zoom").arg(i));
            } else  {
                showError(QString("missing %1 (drop or (throw and lose)) zoom").arg(i));
            }
        }
        if (win[0][i] == 0) {
            if (i == 1 && ! has1) {
                showInfo(QString("missing %1 win no-zoom").arg(i));
            } else  {
                showError(QString("missing %1 win no-zoom").arg(i));
            }
        }
        if (win[1][i] == 0) {
            if (i == 1 && ! has1) {
                showInfo(QString("missing %1 win zoom").arg(i));
            } else  {
                showError(QString("missing %1 win zoom").arg(i));
            }
        }
        if (show_drink[0][i] == 0 && show_drink[1][i] == 0) {
            if (i == 1 && ! has1) {
                showInfo(QString("missing %1 show drink").arg(i));
            } else  {
                showError(QString("missing %1 show drink").arg(i));
            }
        } else {
            if (show_drink[0][i] == 0) {
                showInfo(QString("missing %1 show drink no-zoom").arg(i));
            }
            if (show_drink[1][i] == 0) {
                showInfo(QString("missing %1 show drink zoom").arg(i));
            }
        }
        if (show[0][i] == 0) {
            if (i == 1 && ! has1) {
                showInfo(QString("missing %1 show bare-handed no-zoom").arg(i));
            } else  {
                showError(QString("missing %1 show bare-handed no-zoom").arg(i));
            }
        } else if (show[0][i] < 5) {
            showInfo(QString("(%1 of 5) %2 show bare-handed no-zoom").arg(show[0][i]).arg(i));
        }
        if (show[1][i] == 0) {
            if (i == 1 && ! has1) {
                showInfo(QString("missing %1 show bare-handed zoom").arg(i));
            } else  {
                showError(QString("missing %1 show bare-handed zoom").arg(i));
            }
        } else if (show[1][i] < 5) {
            showInfo(QString("(%1 of 5) %2 show bare-handed zoom").arg(show[1][i]).arg(i));
        }
        if (show_cards[0][i] == 0) {
            showInfo(QString("missing %1 show cards no-zoom").arg(i));
        } else if (show_cards[0][i] < 5) {
            showInfo(QString("(%1 of 5) %2 show cards no-zoom").arg(show_cards[0][i]).arg(i));
        }
        if (show_cards[1][i] == 0) {
            showInfo(QString("missing %1 show cards zoom").arg(i));
        } else if (show_cards[1][i] < 5) {
            showInfo(QString("(%1 of 5) %2 show cards zoom").arg(show_cards[1][i]).arg(i));
        }
    }
    showInfo("Check done");
}

void ClipEditor::slotGenerateExtra() {

    QVector<qint64> lStart;
    QVector<qint64> lEnd;
    QVector<int> lPieces;
    QVector<bool> lZoom;
    QVector<ClipSound> lQuiet;

    QTextDocument *document = ui->editTs->document();
    QTextBlock line = document->firstBlock();

    qint64 time;
    QString action;
    int pieces;
    QString modifier;
    bool zoom;
    ClipSound quiet;
    while (line.isValid() && Util::scanTimestampLine(line.text(), &time, &action, &pieces, &modifier, &zoom, &quiet) != TimestampLineAction) {
        line = line.next();
    }
    if (line.isValid()) {
        QTextBlock next = line.next();
        while (next.isValid()) {
            qint64 nextTime;
            QString nextAction;
            int nextPieces;
            QString nextModifier;
            bool nextZoom;
            ClipSound nextQuiet;
            while(next.isValid() && Util::scanTimestampLine(next.text(), &nextTime, &nextAction, &nextPieces, &nextModifier, &nextZoom, &nextQuiet) != TimestampLineAction) {
                next = next.next();
            }
            if (next.isValid()) {
                if (action == "drop" || action == "win" || action == "lose") {
                //if (action == "drop") {
                //if (quiet && ((action == "drop" && modifier != "throw") || action == "win" || action == "lose")) {
                    lStart.append(time);
                    lEnd.append(nextTime);
                    lPieces.append(pieces);
                    lZoom.append(zoom);
                    lQuiet.append(quiet);
                }

                time = nextTime;
                action = nextAction;
                pieces = nextPieces;
                modifier = nextModifier;
                zoom = nextZoom;
                quiet = nextQuiet;
                line = next;
                next = line.next();
            }
        }
    }

    QString buffer;
    QTextStream collect(&buffer);
    for (int i = 0; i < lStart.length(); i++) {
        collect << Util::formatTsLine(lStart[i], "show", lPieces[i], "", lZoom[i], lQuiet[i]);
        collect << "\n";
        collect << Util::formatTsLine(lEnd[i], "-", lPieces[i], "", false, ClipSoundLoud);
        collect << "\n";
    }
    QTextCursor tc = ui->editTs->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertText(collect.readAll());
}
