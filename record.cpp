#include <algorithm>

#include "record.h"
#include "calculate.h"

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

RecordByMod::RecordByMod(Mod *mod, QString *inputs, QDate date)
    : inputs(inputs), mod(mod){
    set_date(date);
}
enum RECORD_TYPE RecordByMod::get_type() const {
    if(mod->type==1)
        return OBTAIN;
    return CONSUME;
}
bool isoperator(QChar a)
{
    if(a=='='||a=='+'||a=='-'||a=='*'||a=='/'||a=='('||a==')'||a=='<'||a=='>'||a=='!'||a=='^'||a==' '||a==',')
        return true;
    return false;
}
int RecordByMod::get_point() const {
    QString a=mod->get_fun();
    for(int i=0;i<mod->input_num;i++)
    {
        int hh=0;
        while(a.indexOf(mod->variable[i],hh)!=-1)
        {
            int pos=a.indexOf(mod->variable[i],hh);
            int len=(mod->variable[i]).size();
            if(pos - 1 >= 0 && !isoperator(a[pos-1]))
            {
                hh=pos+len;
                continue;
            }
            if(pos + len < a.length() && !isoperator(a[pos+len]))
            {
                hh=pos+len;
                continue;
            }
            a.replace(pos, len, QString::number(inputs[i].toDouble()));
        }
    }
    return calc(a);
}
QString RecordByMod::get_display_name() const {
    QString a=mod->get_name();
    for(int i=0;i<mod->input_num;i++)
    {
        int hh=0;
        while(a.indexOf(mod->variable[i],hh)!=-1)
        {
            int pos=a.indexOf(mod->variable[i],hh);
            int len=(mod->variable[i]).size();
            if(pos - 1 >= 0 && a[pos-1]!='{')
            {
                hh=pos+len;
                continue;
            }
            if(pos + len < a.length() && a[pos+len]!='}')
            {
                hh=pos+len;
                continue;
            }
            a.replace(pos-1, len+2, inputs[i]);
        }
    }
    return a;
}
QString RecordByMod::to_string() const {
    QString result = mod->get_uuid().toString(QUuid::WithoutBraces);
    for(int i=0;i<mod->input_num;i++) {
        result += "\n";
        result += inputs[i];
    }
    return result;
}
void RecordByMod::from_string(std::unordered_map<QString, Mod *> uuid_map, QString str) {
    QStringList list = str.split('\n');

    auto it = uuid_map.find(list[0]);
    mod = (it != uuid_map.end()) ? it->second : nullptr;

    // 存储的变量值数量可能少于模板当前的变量数（旧版本数据），按较大者分配，缺失的补 "0"
    int stored = list.size() - 1;
    int needed = mod ? std::max(stored, mod->input_num) : stored;
    delete[] inputs;
    inputs = new QString[needed];
    for (int i = 0; i < stored; i++) {
        inputs[i] = list[i + 1];
    }
    for (int i = stored; i < needed; i++) {
        inputs[i] = "0";
    }
    // QString a="";
    // int flag=0;
    // int total=0;
    // for(int i=0;i<s.size();i++)
    //     if(s[i]==',')
    //         total++;
    // total--;
    // inputs=new double[total];
    // int cntt=0;
    // for(int i=0;i<s.size();i++)
    // {
    //     if(s[i]!=',')
    //         a=a+s[i];
    //     else
    //     {
    //         if(flag==0)
    //             mod=mods[a.toInt()];
    //         else
    //         {
    //             inputs[cntt++]=a.toDouble();
    //             a="";
    //         }
    //     }
    // }
}
Mod* RecordByMod::get_mod(){
    return mod;
}
void RecordByMod::set_mod(Mod *mod) {
    this->mod = mod;
}
void RecordByMod::set_inputs(QString *inputs) {
    this->inputs = inputs;
}

QString *RecordByMod::get_inputs() {
    return inputs;
}

void RecordByMod::remap_inputs(const std::vector<QString> &old_variables) {
    if (mod == nullptr) return;
    QString *new_inputs = new QString[mod->input_num];
    for (int i = 0; i < mod->input_num; i++) {
        new_inputs[i] = "0";
        for (size_t j = 0; j < old_variables.size(); j++) {
            if (old_variables[j] == mod->variable[i]) {
                new_inputs[i] = inputs[j];
                break;
            }
        }
    }
    delete[] inputs;
    inputs = new_inputs;
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
void RecordDirect::from_string(std::unordered_map<QString, Mod *> uuid_map, QString s) {
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
int MultipleRecord::get_point_sum() {
    int sum = 0;
    for (auto r : *this) {
        sum += r->get_signed_point();
    }
    return sum;
}
QString MultipleRecord::get_display_name() {
    if (empty()) return "";
    if ((*this)[0]->get_class() == BY_MOD) {
        return ((RecordByMod *) (*this)[0])->get_mod()->get_shortname();
    }
    return (*this)[0]->get_display_name();
}
MultipleRecord MultipleRecord::filter(bool (*func) (Record*)) {

}
