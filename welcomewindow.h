#ifndef WELCOMEWINDOW_H
#define WELCOMEWINDOW_H

#include <QMainWindow>

namespace Ui {
class WelcomeWindow;
}

class WelcomeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WelcomeWindow(QWidget *parent = nullptr);
    ~WelcomeWindow();

    bool closed = false;

private:
    Ui::WelcomeWindow *ui;

    void open_login_window();
    void open_record_window();
};

#endif // WELCOMEWINDOW_H
