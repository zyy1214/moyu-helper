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
    std::vector<QString> result;
    QStringList list = str.split('\n');

    mod = uuid_map[list[0]];

    inputs = new QString[list.size() - 1];
    for (int i = 1, len = list.size(); i < len; i++) {
        inputs[i - 1] = list[i];
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
