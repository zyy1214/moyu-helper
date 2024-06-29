#ifndef THEMEWINDOW_H
#define THEMEWINDOW_H

#include <QMainWindow>
#include "recordwindow.h"
namespace Ui {
class ThemeWindow;
}

class ThemeWindow : public QMainWindow
{
    Q_OBJECT
    QMainWindow* mainwindow;

public:
    explicit ThemeWindow(QMainWindow* mainwindow,RecordWindow* rww,QWidget *parent = nullptr);
    ~ThemeWindow();

private slots:
    void on_themebutton1_clicked();

    void on_themebutton2_clicked();

    void on_themebutton3_clicked();

    void on_themebutton4_clicked();

    void on_night_clicked();

private:
    Ui::ThemeWindow *ui;
    RecordWindow* rw;
    int flag=0;
};

#endif // THEMEWINDOW_H
