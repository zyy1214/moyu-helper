#ifndef THEMEWINDOW_H
#define THEMEWINDOW_H

#include <QMainWindow>

namespace Ui {
class ThemeWindow;
}

class ThemeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ThemeWindow(QWidget *parent = nullptr);
    ~ThemeWindow();

private slots:
    void on_themebutton1_clicked();

    void on_themebutton2_clicked();

    void on_themebutton3_clicked();

    void on_themebutton4_clicked();

private:
    Ui::ThemeWindow *ui;
};

#endif // THEMEWINDOW_H
