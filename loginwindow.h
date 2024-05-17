#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QMainWindow>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private:
    QStackedWidget *stackedWidget;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *registerUsernameEdit;
    QLineEdit *registerPasswordEdit;
    QLineEdit *confirmPasswordEdit;
    QPushButton *loginButton;
    QPushButton *registerButton;
private slots:
    void skipButtonClicked();
    void login_clicked();
    void register_clicked();
};

#endif // LOGINWINDOW_H
