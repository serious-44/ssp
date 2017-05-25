#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>

#include <configfile.h>
#include <log.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("SeriousStripPoker");
    QCoreApplication::setApplicationName("SeriousStripPoker");

    qRegisterMetaType<QVector<int> >("QVector<int>");
    qRegisterMetaType<QVector<double> >("QVector<double>");

    time_t t;
    srand((unsigned) time(&t));

    QApplication a(argc, argv);
    a.setStyle("fusion");

    int r1 = QFontDatabase::addApplicationFont(":/fonts/UbuntuMono-R.ttf");
    int r2 = QFontDatabase::addApplicationFont(":/fonts/Ubuntu-R.ttf");

    ConfigFile *myConfig = new ConfigFile(argv[0], false);
    Log *myLog = new Log(myConfig);

    MainWindow w(myConfig, myLog);
    w.show();

    return a.exec();
}
