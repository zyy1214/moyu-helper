#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

#include "recordwindow.h"
#include "modwindow.h"
#include "chartwindow.h"
#include "themewindow.h"

#include "data_storage.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Data *data;

private slots:
    void on_button_menu_clicked();

    void on_button_mod_clicked();

    void on_button_record_clicked();

    void on_button_chart_clicked();

    void on_button_theme_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::MainWindow *ui;
    RecordWindow *rw;
    ModWindow *mw;
    ChartWindow *cw;
    ThemeWindow *tw;
    QSystemTrayIcon *trayIcon;

    void clear_page_choice();
};

#endif // MAINWINDOW_H
