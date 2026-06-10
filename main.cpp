#include "welcomewindow.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("moyu-helper");
    QCoreApplication::setApplicationName("moyu-helper");

    QApplication a(argc, argv);

    WelcomeWindow w;
    if (!w.closed) w.show();

    return a.exec();
}
