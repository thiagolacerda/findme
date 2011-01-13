#include <QtGui/QApplication>
#include <QTimer>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow *w = new MainWindow();
    w->initMessagingMode();
    //w->initLocationMode();
    //w->connectSignals();
    //w->startGPS();

#if defined(Q_WS_S60)
    w->showMaximized();
#else
    w->show();
#endif
    return a.exec();
}
