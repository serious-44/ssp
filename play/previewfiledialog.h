#ifndef PREVIEWFILEDIALOG_H
#define PREVIEWFILEDIALOG_H

#include <QFileDialog>
#include <QDir>
#include <QStringList>

#include "backgroundwidget.h"

class PreviewFileDialog : public QFileDialog
{
    Q_OBJECT
public:
    PreviewFileDialog(QWidget *parent = Q_NULLPTR, const QString &caption = QString(), const QString &directory = QString(), const QString &filter = QString());
    ~PreviewFileDialog();

private slots:
   void CheckChange();
   void CheckChangeAsync();

private:
    bool directoryChangeCheckPending;
    QStringList lastSelectedFiles;
    QDir lastDirectory;
    QWidget *extras;
    BackgroundWidget *images[5];
};

#endif // PREVIEWFILEDIALOG_H
