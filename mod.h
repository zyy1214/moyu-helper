#ifndef MOD_H
#define MOD_H


#include <QDate>
#include <QString>
#include <QUuid>
#include <vector>

enum RECORD_TYPE {
    OBTAIN = 1, CONSUME = 2
};


class Formula{ // wzl 负责
public:
    QString func;
    std::vector<QString> var;
    Formula(QString a);
    operator QString();
    bool right_formula();//公式是否合法
    static int what_type(QString a);
};

class Mod {
    int id;
    QUuid uuid;
    bool deleted; // 用户修改模板时，可以选择保留之前的记录不被修改，此时该模板状态为 deleted，不在模板列表里显示
    QString name; // 记录显示出来的信息
    QString short_name;//别名
    QString name_merge; // 记录同一个模板的记录合并后展示的信息
    // 例：使用手机 {x} 分钟，使用电脑 {y} 分钟
    // 2024年4月 "共计使用手机 {x} 分钟，使用电脑 {y} 分钟。“
    Formula *formula; // 用于计算积分的公式


    //////////////////////////////////////////////////////////////

public:
    enum RECORD_TYPE type = OBTAIN ;// 或者 TYPE_CONSUME; // 模板类型
    int input_num; // 代表该条记录需要几个变量
    std::vector<QString> variable; //记录变量的字符串
    std::vector<QString> labels;

    Mod(int mod_id, QString a, Formula *b, enum RECORD_TYPE type, QString shortn);
    void change(QString a, Formula *b, enum RECORD_TYPE type,QString shortn);//更改模板
    std::vector<QString> get_labels();//获取全部标签
    bool find_label(QString label);//查找一个标签是否在这个模板里
    bool is_deleted();//是否被删除
    bool search_str(QString aa); //查找一个字符串aa是否在name中
    QString get_name();//输出名字
    QString get_shortname();//输出别名
    QString get_fun();//输出公式

    bool name_legal(); //判断名字是否合法

    enum RECORD_TYPE get_type() {
        return type;
    }

    void set_deleted(bool deleted) {
        this->deleted = deleted;
    }

    QUuid get_uuid() {
        return uuid;
    }
    void set_uuid(QString uuid) {
        this->uuid = QUuid(uuid);
    }
    void create_uuid() {
        uuid = QUuid::createUuid();
    }

    QString get_name_merge() {
        return name_merge;
    }
    void set_name_merge(QString name_merge) {
        this->name_merge = name_merge;
    }

    QString get_formula_text() {
        return formula->func;
    }
    void set_formula_text(QString formula);

    QString get_labels_string() {
        QString result;
        for (const QString &str : labels) {
            result.append(str).append('\n');
        }
        // 删除最后一个多余的换行符
        if (!result.isEmpty()) {
            result.chop(1);
        }
        return result;
    }
    void set_labels_string(QString labels_string) {
        labels.clear();
        QStringList list = labels_string.split('\n');
        for (const QString &s : list) {
            if (s != "") {
                labels.push_back(s);
            }
        }
    }

    int get_id() {
        return id;
    }
    void set_id(int id) {
        this->id = id;
    }
    void set_name(QString name) {
        this->name = name;
    }
    void set_short_name(QString name) {
        this->short_name = name;
    }
    void set_type(enum RECORD_TYPE type) {
        this->type = type;
    }

};

#endif // MOD_H
