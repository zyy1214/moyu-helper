#ifndef THEMEWINDOW_H
#define THEMEWINDOW_H

#include <QMainWindow>
namespace Ui {
class ThemeWindow;
}

class ThemeWindow : public QMainWindow
{
    Q_OBJECT
    QMainWindow* mainwindow;

public:
    explicit ThemeWindow(QMainWindow* mainwindow, QWidget *parent = nullptr);
    ~ThemeWindow();

private slots:
    void on_themebutton1_clicked();

    void on_themebutton2_clicked();

    void on_themebutton3_clicked();

    void on_themebutton4_clicked();

    void on_night_clicked();

private:
    Ui::ThemeWindow *ui;
    bool dark_mode = false;
};

#endif // THEMEWINDOW_H
