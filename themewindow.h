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

private:
    Ui::ThemeWindow *ui;
};

#endif // THEMEWINDOW_H
