#include "remotecontrolwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RemoteControlWindow w;
    w.show();

    return a.exec();
}
