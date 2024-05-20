#include "record.h"
#include "calculate.h"
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


RecordByMod::RecordByMod(Mod *mod, double *inputs, QDate date)
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
            a.replace(pos,len,QString::number(inputs[i]));
        }
    }
    return calc(a);
}
QString RecordByMod::get_display_name() const {
    return mod->get_shortname();
}
QString RecordByMod::to_string() const {
    // QString a=QString::number(mod->id);
    // for(int i=0;i<mod->input_num;i++)
    // {
    //     a=a+",";
    //     a=a+QString::number(inputs[i]);
    // }
    // a=a+",";
    // return a;
    return "";
}
void RecordByMod::from_string(QString s) {
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
void RecordByMod::set_inputs(double *inputs) {
    this->inputs = inputs;
}

double *RecordByMod::get_inputs() {
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
