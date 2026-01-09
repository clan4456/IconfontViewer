#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    if (argc == 2)
    {
        w.showFont(argv[1]);
    }
    return app.exec();
}