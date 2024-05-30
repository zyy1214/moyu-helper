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

    if (get_value("token") == "") {
        open_login_window();
    } else {
        Network *n = new Network(this, "https://geomedraw.com/qt/verify_token");
        n->add_data("username", get_value("username"));
        n->add_data("token", get_value("token"));
        n->post([=] (void *window, QString reply) {
            if (reply == "true") {
                ((WelcomeWindow *) window)->open_record_window();
            } else {
                ((WelcomeWindow *) window)->open_login_window();
            }
        }, [=] (QMainWindow *window) {
            ((WelcomeWindow *) window)->open_login_window();
        });
    }
}

WelcomeWindow::~WelcomeWindow()
{
    delete ui;
}
