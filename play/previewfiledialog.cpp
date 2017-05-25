#include "previewfiledialog.h"

#include "util.h"

#include <QDebug>
#include <QTimer>
#include <QBoxLayout>
#include <QGridLayout>

PreviewFileDialog::PreviewFileDialog(QWidget *parent, const QString &caption, const QString &directory, const QString &filter) :
    QFileDialog(parent, caption, directory, filter),
    directoryChangeCheckPending(false)
{
    setOption(DontUseNativeDialog);
    setViewMode(Detail);

    connect(this, SIGNAL(currentChanged(const QString &)), this, SLOT(CheckChangeAsync()));

    extras = new QWidget(this);
    extras->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    extras->setMinimumHeight(150);
    //extras->setStyleSheet("background:red");
    QBoxLayout *extrasLayout = new QHBoxLayout(extras);
    extrasLayout->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < 5; i++) {
        images[i] = new BackgroundWidget(extras);
        extrasLayout->addWidget(images[i]);
    }

    QGridLayout * gl = dynamic_cast<QGridLayout*>(layout());
    if (gl)
    {
       int r = gl->rowCount();
       gl->addWidget(extras, r, 0, 1, gl->columnCount());
       gl->setRowStretch(r, 0);
    }
}

PreviewFileDialog::~PreviewFileDialog() {

}

void PreviewFileDialog::CheckChange() {
    directoryChangeCheckPending = false;

    QString fileNames[5];

    QDir curDir = directory();
    if (curDir != lastDirectory)
    {
       lastDirectory = curDir;
       //qDebug() << "directory" << lastDirectory;
    }

    QStringList fileNamesRaw = selectedFiles();
    if (fileNamesRaw != lastSelectedFiles)
    {
        lastSelectedFiles = fileNamesRaw;
        QStringList tsNames = Util::expandCompilations(fileNamesRaw);
        //qDebug() << "files" << tsNames;
        for (int i = 0; i < tsNames.length() && i < 5; i++) {
            if (tsNames[i].endsWith(".ts")) {
                QString jpg = tsNames[i];
                jpg.replace(Util::regPatternTs, ".jpg");
                QFileInfo fi(jpg);
                if (fi.isFile()) {
                    fileNames[i] = jpg;
                } else {
                    QFileInfo fi2(tsNames[i]);
                    if (fi2.isFile()) {
                        fileNames[i] = "://misc/no_preview_filedialog.png";
                    }
                }
            }
        }
        for (int i = 0; i < 5; i++) {
            if (fileNames[i].isEmpty()) {
                images[i]->init(0);
            } else {
                images[i]->init(fileNames[i]);
            }
        }
    }
}

void PreviewFileDialog::CheckChangeAsync() {
    //qDebug() << "check";
    if (!directoryChangeCheckPending)
    {
       directoryChangeCheckPending = true;
       QTimer::singleShot(0, this, SLOT(CheckChange()));
    }

}
