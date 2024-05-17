#include "recordwindow.h"
#include "loginwindow.h"
#include "mainwindow.h"
#include "welcomewindow.h"
#include "ui_welcomewindow.h"

#include "data_storage.h"
#include "network.h"


void open_login_window(WelcomeWindow *window) {
    LoginWindow *w = new LoginWindow();
    w->show();
    window->close();
}

void open_record_window(WelcomeWindow *window) {
    // RecordWindow *w = new RecordWindow();
    MainWindow *w = new MainWindow();
    w->show();
    window->close();
}

WelcomeWindow::WelcomeWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WelcomeWindow)
{
    setWindowOpacity(0);
    ui->setupUi(this);

    if (get_value("token") == "") {
        open_login_window(this);
    } else {
        Network *n = new Network(this, "https://geomedraw.com/qt/verify_token");
        n->add_data("username", get_value("username"));
        n->add_data("token", get_value("token"));
        n->post([=] (QMainWindow *window, QString reply) {
            if (reply == "true") {
                open_record_window((WelcomeWindow *) window);
            } else {
                open_login_window((WelcomeWindow *) window);
            }
        }, [=] (QMainWindow *window) {
            open_login_window((WelcomeWindow *) window);
        });
    }

    //MainWindow w;
}

WelcomeWindow::~WelcomeWindow()
{
    delete ui;
}
