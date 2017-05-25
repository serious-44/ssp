#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "util.h"

MainWindow::MainWindow(ConfigFile *c, Log *l, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    configFile(c),
    log(l),
    editWindow(0),
    previewProducer(0)
{
    ui->setupUi(this);
    ui->gameTable->init(configFile, log);
    ui->video1->init(1);
    ui->video2->init(2);
    ui->video3->init(3);
    ui->video4->init(4);
    ui->video5->init(5);

    connect(ui->gameTable, SIGNAL(signalPlayerSelected(int, QString)), ui->video1, SLOT(slotPlayerSelected(int, QString)));
    connect(ui->gameTable, SIGNAL(signalPlayerSelected(int, QString)), ui->video2, SLOT(slotPlayerSelected(int, QString)));
    connect(ui->gameTable, SIGNAL(signalPlayerSelected(int, QString)), ui->video3, SLOT(slotPlayerSelected(int, QString)));
    connect(ui->gameTable, SIGNAL(signalPlayerSelected(int, QString)), ui->video4, SLOT(slotPlayerSelected(int, QString)));
    connect(ui->gameTable, SIGNAL(signalPlayerSelected(int, QString)), ui->video5, SLOT(slotPlayerSelected(int, QString)));
    connect(ui->gameTable, SIGNAL(signalPlayerAction(int, JobAction, int, bool)), ui->video1, SLOT(slotPlayerAction(int, JobAction, int, bool)));
    connect(ui->gameTable, SIGNAL(signalPlayerAction(int, JobAction, int, bool)), ui->video2, SLOT(slotPlayerAction(int, JobAction, int, bool)));
    connect(ui->gameTable, SIGNAL(signalPlayerAction(int, JobAction, int, bool)), ui->video3, SLOT(slotPlayerAction(int, JobAction, int, bool)));
    connect(ui->gameTable, SIGNAL(signalPlayerAction(int, JobAction, int, bool)), ui->video4, SLOT(slotPlayerAction(int, JobAction, int, bool)));
    connect(ui->gameTable, SIGNAL(signalPlayerAction(int, JobAction, int, bool)), ui->video5, SLOT(slotPlayerAction(int, JobAction, int, bool)));
    connect(ui->video1, SIGNAL(signalPlayerActionDone(int)), ui->gameTable, SLOT(slotPlayerActionDone(int)));
    connect(ui->video1, SIGNAL(info(QString)), ui->gameTable, SLOT(slotShowInfo(QString)));
    connect(ui->video1, SIGNAL(error(QString)), ui->gameTable, SLOT(slotShowError(QString)));
    connect(ui->video1, SIGNAL(fatalError(QString)), ui->gameTable, SLOT(slotShowFatalError(QString)));
    connect(ui->video2, SIGNAL(signalPlayerActionDone(int)), ui->gameTable, SLOT(slotPlayerActionDone(int)));
    connect(ui->video2, SIGNAL(info(QString)), ui->gameTable, SLOT(slotShowInfo(QString)));
    connect(ui->video2, SIGNAL(error(QString)), ui->gameTable, SLOT(slotShowError(QString)));
    connect(ui->video2, SIGNAL(fatalError(QString)), ui->gameTable, SLOT(slotShowFatalError(QString)));
    connect(ui->video3, SIGNAL(signalPlayerActionDone(int)), ui->gameTable, SLOT(slotPlayerActionDone(int)));
    connect(ui->video3, SIGNAL(info(QString)), ui->gameTable, SLOT(slotShowInfo(QString)));
    connect(ui->video3, SIGNAL(error(QString)), ui->gameTable, SLOT(slotShowError(QString)));
    connect(ui->video3, SIGNAL(fatalError(QString)), ui->gameTable, SLOT(slotShowFatalError(QString)));
    connect(ui->video4, SIGNAL(signalPlayerActionDone(int)), ui->gameTable, SLOT(slotPlayerActionDone(int)));
    connect(ui->video4, SIGNAL(info(QString)), ui->gameTable, SLOT(slotShowInfo(QString)));
    connect(ui->video4, SIGNAL(error(QString)), ui->gameTable, SLOT(slotShowError(QString)));
    connect(ui->video4, SIGNAL(fatalError(QString)), ui->gameTable, SLOT(slotShowFatalError(QString)));
    connect(ui->video5, SIGNAL(signalPlayerActionDone(int)), ui->gameTable, SLOT(slotPlayerActionDone(int)));
    connect(ui->video5, SIGNAL(info(QString)), ui->gameTable, SLOT(slotShowInfo(QString)));
    connect(ui->video5, SIGNAL(error(QString)), ui->gameTable, SLOT(slotShowError(QString)));
    connect(ui->video5, SIGNAL(fatalError(QString)), ui->gameTable, SLOT(slotShowFatalError(QString)));

    connect(ui->gameTable, SIGNAL(signalPlayerAction(int, JobAction, int, bool)), &soundPlayer, SLOT(slotPlayerAction(int, JobAction, int, bool)));
    connect(&soundPlayer, SIGNAL(info(QString)), ui->gameTable, SLOT(slotShowInfo(QString)));
    connect(&soundPlayer, SIGNAL(error(QString)), ui->gameTable, SLOT(slotShowError(QString)));
    connect(&soundPlayer, SIGNAL(fatalError(QString)), ui->gameTable, SLOT(slotShowFatalError(QString)));

    connect( ui->gameTable->ui->buttonEdit, SIGNAL(clicked()), this, SLOT (slotButtonEditClicked()));
    connect( ui->gameTable->ui->buttonGeneratePreviews, SIGNAL( clicked()), this, SLOT (slotButtonPreviewsClicked()));
    connect( ui->gameTable->ui->pushButton_Exit, SIGNAL(clicked()), this, SLOT (close()));

    QSettings settings;
    if (settings.contains("mainwindow/splitter")) {
        QRect geometry = settings.value("mainwindow/geometry").toRect();
        bool fullScreen = settings.value("mainwindow/fullScreen").toBool();
        setGeometry(geometry);
        if (fullScreen) {
            setWindowState(Qt::WindowFullScreen);
        }
        ui->splitter->restoreState(settings.value("mainwindow/splitter").toByteArray());
        ui->splitter1->restoreState(settings.value("mainwindow/splitter1").toByteArray());
        ui->splitter2->restoreState(settings.value("mainwindow/splitter2").toByteArray());
        ui->splitter3->restoreState(settings.value("mainwindow/splitter3").toByteArray());
    }
}

MainWindow::~MainWindow()
{
    if (editWindow) {
        delete editWindow;
    }
    if (previewProducer) {
        delete previewProducer;
    }
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    bool saveText = editWindow != Q_NULLPTR && editWindow->needsSave();
    bool inGame = ui->gameTable->isInGame();

    bool save = false;
    if (saveText || inGame) {
        QIcon icon(":/misc/program.png");
        QMessageBox msgBox;
        msgBox.setWindowIcon(icon);
        msgBox.setText("Really Close?");
        msgBox.setWindowTitle("Serious!");
        if (saveText && inGame) {
            msgBox.setInformativeText("Do you want to save the edited ts file?\nDo you want to close the current game?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Close | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
        } else if (saveText) {
            msgBox.setInformativeText("Do you want to save the edited ts file?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Close | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
        } else {
            msgBox.setInformativeText("Do you want to close the current game?");
            msgBox.setStandardButtons(QMessageBox::Close | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Close);
        }
        int ret = msgBox.exec();
        if (ret ==  QMessageBox::Save) {
            save = true;
        } else if (ret == QMessageBox::Cancel) {
            event->ignore();
            return;
        }
    }

    ui->gameTable->prepareClose();
    if (editWindow) {
        editWindow->prepareClose(save);
        editWindow->close();
    }
    if (previewProducer) {
        previewProducer->prepareClose();
        previewProducer->close();
    }
    QSettings settings;
    settings.setValue("mainwindow/geometry", geometry());
    settings.setValue("mainwindow/fullScreen", isFullScreen());
    settings.setValue("mainwindow/splitter", ui->splitter->saveState());
    settings.setValue("mainwindow/splitter1", ui->splitter1->saveState());
    settings.setValue("mainwindow/splitter2", ui->splitter2->saveState());
    settings.setValue("mainwindow/splitter3", ui->splitter3->saveState());

    event->accept();
}

void MainWindow::slotButtonEditClicked() {
    if (!editWindow) {
        editWindow = new ClipEditor();
    }
    editWindow->show();
    editWindow->raise();
}

void MainWindow::slotButtonPreviewsClicked() {
    if (!previewProducer) {
        previewProducer = new PreviewProducer();
    }
    previewProducer->show();
    previewProducer->raise();
}
