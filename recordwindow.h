#ifndef RECORDWINDOW_H
#define RECORDWINDOW_H

#include <QMainWindow>

#include "data_storage.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class RecordWindow;
}
QT_END_NAMESPACE

class RecordWindow : public QMainWindow
{
    Q_OBJECT

public:
    RecordWindow(Data *data, QWidget *parent = nullptr);
    ~RecordWindow();
    void setup_total_points();

    void setup_records();
    void add_mr_items(QLayout *layout, QDate date, int num);

    Data *data;

private slots:
    void on_add_record_clicked();

    void on_date_selector_selectionChanged();

    void on_option_by_day_pressed();

    void on_option_by_week_clicked();

    void on_option_by_month_clicked();

    void on_option_by_year_clicked();

    void on_record_added(Record *record);
    void on_record_modified(Record *record);
    void on_record_deleted(Record *record);
    void on_all_record_changed();

private:
    Ui::RecordWindow *ui;

    void clear_option_choose();

    void init_data();

};
#endif // RECORDWINDOW_H
