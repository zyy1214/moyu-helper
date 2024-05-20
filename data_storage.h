#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include "record.h"

void load_data(std::map<QDate, MultipleRecord *, std::greater<QDate>> &records);
int db_add_record(Record *record); // 返回数据的 id
bool db_modify_record(Record *record);
bool db_delete_record(Record *record);
void save_value(QString key, QString value);
QString get_value(QString key);

class Data {
public:
    std::map<QDate, MultipleRecord *, std::greater<QDate>> records;
    std::vector<Mod*> mods;//模板
    std::vector<QString> totallabels;// 存放所有标签
    int total_points = 0;
    int last_week_points = 0;
};

#endif // DATA_STORAGE_H
