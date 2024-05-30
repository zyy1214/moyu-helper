#include "welcomewindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    WelcomeWindow w;
    if (!w.closed) w.show();

    return a.exec();
}
