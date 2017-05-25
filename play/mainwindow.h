#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <gametableimpl.h>
#include <configfile.h>
#include <log.h>
#include <clipeditor.h>
#include <previewproducer.h>
#include <soundplayer.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(ConfigFile *c, Log *l, QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slotButtonEditClicked();
    void slotButtonPreviewsClicked();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    ConfigFile *configFile;
    Log *log;
    ClipEditor *editWindow;
    PreviewProducer *previewProducer;
    SoundPlayer soundPlayer;

};

#endif // MAINWINDOW_H
