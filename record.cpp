#include "record.h"

class Formula; // wzl 负责

/*Record::Record() {
    id = ++current_max_id;
}
int Record::current_max_id = 0;*/

Date::Date(int y, int m, int d): y(y), m(m), d(d) {}
bool Date::operator < (const Date &date) const {
    if (y == date.y) {
        if (m == date.m) {
            return d > date.d;
        }
        return m > date.m;
    }
    return y > date.y;
}
int Date::get_weekday() const {
    return (d + 2 * m + int(3 * (m + 1) / 5) + y + int(y / 4) - int(y / 100) + int(y / 400)) % 7;
}

// QString Mod::get_name() {
//     return "";
// }


// RecordByMod::RecordByMod(Mod *mod, std::vector<double> variables, QDate date) {
//     set_date(date);
// }
// enum RECORD_TYPE RecordByMod::get_type() const {
//     return mod->get_type();
// }
// int RecordByMod::get_point() const {
//     return 0;
// }
// QString RecordByMod::get_display_name() const {
//     return mod->get_name();
// }
// QString RecordByMod::to_string() const {
//     return "";
// }
// void RecordByMod::from_string(QString s) {

// }
Mod* RecordByMod::get_mod(){
        return mod;
}

RecordDirect::RecordDirect(QString name, enum RECORD_TYPE type, int point, QDate date): name(name), type(type), point(point) {
    set_date(date);
}
enum RECORD_TYPE RecordDirect::get_type() const {
    return type;
}
int RecordDirect::get_point() const {
    return point;
}
QString RecordDirect::get_display_name() const {
    return name;
}
QString RecordDirect::to_string() const {
    return name;
}
void RecordDirect::from_string(QString s) {
    name = s;
}
void RecordDirect::set_type(enum RECORD_TYPE type) {
    this->type = type;
}
void RecordDirect::set_point(int point) {
    this->point = point;
}
void RecordDirect::set_name(QString name) {
    this->name = name;
}

void MultipleRecord::add_record(Record *record) {
    push_front(record);
    //push_back(record);
}
void MultipleRecord::delete_record(int index) {
    erase(begin() + index);
}
void MultipleRecord::modify_record(int index, Record *record) {

}
MultipleRecord MultipleRecord::filter(bool (*func) (Record*)) {

}
