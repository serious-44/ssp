#include "mainwindow.h"
#include <QApplication>

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

    ConfigFile *myConfig = new ConfigFile(argv[0], false);
    Log *myLog = new Log(myConfig);

    MainWindow w(myConfig, myLog);
    w.show();

    return a.exec();
}
