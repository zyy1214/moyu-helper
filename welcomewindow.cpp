#include "loginwindow.h"
#include "mainwindow.h"
#include "welcomewindow.h"
#include "ui_welcomewindow.h"

#include "data_storage.h"
#include "network.h"


void WelcomeWindow::open_login_window() {
    LoginWindow *w = new LoginWindow();
    w->show();
    closed = true;
    close();
}

void WelcomeWindow::open_record_window() {
    MainWindow *w = new MainWindow();
    w->show();
    closed = true;
    close();
}

WelcomeWindow::WelcomeWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WelcomeWindow)
{
    setWindowOpacity(0);
    ui->setupUi(this);

    if (get_value("skipped_login") == "true") {
        open_record_window();
    } else if (get_value("token") == "") {
        open_login_window();
    } else {
        Network *n = new Network("https://geomedraw.com/qt/verify_token");
        n->add_data("username", get_value("username"));
        n->add_data("token", get_value("token"));
        n->post(2000);
        connect(n, &Network::succeeded, [=] (QString reply) {
            if (reply == "true") {
                open_record_window();
            } else {
                open_login_window();
            }
        });
        connect(n, &Network::failed, [=] () {
            // open_login_window();
            open_record_window();
        });
    }
}

WelcomeWindow::~WelcomeWindow()
{
    delete ui;
}
