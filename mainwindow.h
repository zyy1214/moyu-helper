#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "recordwindow.h"
#include "modwindow.h"
#include "chartwindow.h"

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

private slots:
    void on_button_menu_clicked();

    void on_button_mod_clicked();

    void on_button_record_clicked();

    void on_button_chart_clicked();

private:
    Ui::MainWindow *ui;
    Data *data;
    RecordWindow *rw;
    ModWindow *mw;
    ChartWindow *cw;

    void clear_page_choice();
};

#endif // MAINWINDOW_H
