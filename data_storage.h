#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include "record.h"

class Data : public QObject{
    Q_OBJECT
public:
    std::map<QDate, MultipleRecord *, std::greater<QDate>> records;
    std::vector<Mod*> mods;//模板
    std::vector<QString> totallabels;// 存放所有标签
    int total_points = 0;
    int last_week_points = 0;

signals:
    void mod_added(Mod *mod);
    void mod_modified(Mod *mod);
    void all_mod_changed();
    void label_modified(Mod *mod);
    void record_added(Record *record);
    void record_modified(Record *record);
    void record_deleted(Record *record);
    void all_record_changed();

    void mod_sync_finished();

    void settings_changed();

public slots:
    void sync_records();
};

void load_data(Data *data);
int db_add_record(Data *data, Record *record, bool record_operation = true); // 返回数据的 id
bool db_modify_record(Data *data, Record *record, bool record_operation = true);
bool db_delete_record(Data *data, Record *record, bool record_operation = true);

int db_add_mod(Data *data, Mod *mod, bool record_operation = true);
bool db_modify_mod(Data *data, Mod *mod, bool record_operation = true);
bool db_delete_mod(Data *data, Mod *mod, bool record_operation = true);

void save_value(QString key, QString value, bool user_specific = false);
QString get_value(QString key, bool user_specific = false, QString default_value = "");

void sync_data(Data *data);
void sync_mods_data(Data *data);
void sync_records_data(Data *data);

void sync_settings(Data *data);

#endif // DATA_STORAGE_H
