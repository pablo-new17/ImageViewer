#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationVersion(QString(APP_VERSION));

    MainWindow w;
    w.show();
    return a.exec();
}
