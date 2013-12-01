#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("ratnik");
    a.setOrganizationDomain("rutracker.org");
    a.setApplicationName("Torrents dump viewer");
#ifndef __APPLE__
    a.setWindowIcon(QIcon(":favicon.ico"));
#endif
    MainWindow w;
    w.setWindowState(w.windowState() | Qt::WindowMaximized);
    w.show();
    return a.exec();
}
