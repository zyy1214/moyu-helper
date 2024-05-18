#ifndef RECORD_H
#define RECORD_H

#include <QDate>
#include <QString>
#include <QUuid>
#include <deque>

#include "mod.h"

class Formula; // wzl 负责

enum RECORD_CLASS {
    BY_MOD = 1, DIRECT = 2
};

/*
class Mod {
    int id;
    bool deleted; // 用户修改模板时，可以选择保留之前的记录不被修改，此时该模板状态为 deleted，不在模板列表里显示
    std::vector<QString> labels;
    enum RECORD_TYPE type; // 模板类型
    int input_num; // 代表该条记录需要几个变量
    QString name; // 记录显示出来的信息
    QString name_sum; // 记录同一个模板的记录合并后展示的信息
    Formula *formula; // 用于计算积分的公式
public:
    enum RECORD_TYPE get_type() {
        return type;
    }
    QString get_name();
    std::vector<QString> get_labels();
    std::vector<QString> get_variable_names();
};
*/

// zyy 负责
class Record {
    static int current_max_id;
    int id; // 用于标识本地数据库中的 id
    QUuid uuid; // 用于云端同步标识的 id
    long long created_time; // 记录创建的时间戳
    QDate date; // 该条记录所在的日期
public:
    int get_id() {
        return id;
    }
    QUuid &get_uuid() {
        return uuid;
    }
    virtual enum RECORD_CLASS get_class() const = 0;
    virtual enum RECORD_TYPE get_type() const = 0;
    virtual int get_point() const = 0;
    virtual QString get_display_name() const = 0;
    // 以下两个函数用于序列化、反序列化每种类型的 record 中存储的信息，以存入数据库中或从数据库中取出
    virtual QString to_string() const = 0;
    virtual void from_string(QString s) = 0;
    virtual bool is_from_template() {
        return false;
    }
    int get_signed_point() {
        return get_type() == OBTAIN ? get_point() : -get_point();
    }
    void set_id(int id) {
        this->id = id;
    }
    void set_uuid(QString uuid) {
        this->uuid = QUuid(uuid);
    }
    void create_uuid() {
        uuid = QUuid::createUuid();
    }
    void set_date(QDate &date) {
        this->date = date;
    }
    QDate &get_date() {
        return date;
    }
    Record() {}
};


class RecordByMod: public Record {
    Mod *mod;
    double *inputs;
public:
    RecordByMod(Mod *mod, double *inputs, QDate date);
    virtual enum RECORD_CLASS get_class() const {
        return BY_MOD;
    }
    Mod* get_mod();
    virtual enum RECORD_TYPE get_type() const;
    virtual int get_point() const;
    virtual QString get_display_name() const;
    virtual QString to_string() const;
    virtual void from_string(QString s);
    virtual bool is_from_template() {
        return true;
    }
    void set_mod(Mod *mod);
    void set_inputs(double *inputs);
    double *get_inputs();
};

class RecordDirect: public Record {
    QString name;
    enum RECORD_TYPE type;
    int point;
public:
    RecordDirect(QString name, enum RECORD_TYPE type, int point, QDate date);
    virtual enum RECORD_CLASS get_class() const {
        return DIRECT;
    }
    virtual enum RECORD_TYPE get_type() const;
    virtual int get_point() const;
    virtual QString get_display_name() const;
    virtual QString to_string() const;
    virtual void from_string(QString s);
    void set_type(enum RECORD_TYPE type);
    void set_point(int point);
    void set_name(QString name);
};

// zyy 负责
class Date {
public:
    int y, m, d;
    Date(int y, int m, int d);
    bool operator < (const Date &date) const;
    int get_weekday() const;
};

// zyy 负责
// 同时存储多条记录
class MultipleRecord: public std::deque<Record*> {
public:
    void add_record(Record *record);
    void delete_record(int index);
    void modify_record(int index, Record *record);
    MultipleRecord filter(bool (*func) (Record*));
};

#endif // RECORD_H
