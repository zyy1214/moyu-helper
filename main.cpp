#include "recordwindow.h"
#include "loginwindow.h"
#include "welcomewindow.h"

#include "data_storage.h"
#include "network.h"

#include <QApplication>
#include <QEventLoop>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    WelcomeWindow w;
    w.show();

    return a.exec();
}
