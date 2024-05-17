#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QMainWindow>

#include "data_storage.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ChartWindow;
}
QT_END_NAMESPACE

class ChartWindow : public QMainWindow
{
    Q_OBJECT

public:
    ChartWindow(Data *data, QWidget *parent = nullptr);
    ~ChartWindow();

    Data *data;

private slots:

    void on_select_label_currentIndexChanged(int index);

    void on_select_mod_currentIndexChanged(int index);

    void on_select_graph_currentIndexChanged(int index);

    void on_select_type_currentIndexChanged(int index);

    void on_select_time_currentIndexChanged(int index);

    void on_date_start_userDateChanged(const QDate &date);

    void on_date_end_userDateChanged(const QDate &date);

private:
    Ui::ChartWindow *_ui;
};
#endif // CHARTWINDOW_H
