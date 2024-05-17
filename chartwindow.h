#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_select_label_currentIndexChanged(int index);

    void on_select_mod_currentIndexChanged(int index);

    void on_select_graph_currentIndexChanged(int index);

    void on_select_type_currentIndexChanged(int index);

    void on_select_time_currentIndexChanged(int index);

    void on_date_start_userDateChanged(const QDate &date);

    void on_date_end_userDateChanged(const QDate &date);

private:
    Ui::MainWindow *_ui;
};
#endif // CHARTWINDOW_H
