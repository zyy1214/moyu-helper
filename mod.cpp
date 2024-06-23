#include <QString>
#include <QDebug>
#include <vector>
#include "mod.h"

Formula::Formula(QString a)
{
    func=a;
}
QString Mod::get_fun()
{
    return formula->func;
}

Formula::operator QString(){
    return func;
}




int Formula::what_type(QString a)
{
    if(a=="==" || a=="^" || a=="*" || a=="/" || a=="!=" || a==">=" || a=="<=" ||a=="<" || a==">" || a=="||" || a=="&&")
        return 2; //二元操作符类型
    if(a=="+"||a=="-")
        return 3; //这两个比较特殊
    if(a=="!")
        return 1;
    return 0;
}


void Mod::change(QString a, Formula *b, enum RECORD_TYPE type,QString shortn){ //一个id，一个 使用手机 {} 分钟，使用电脑 {} 分钟 ，一个公式,一个类型
    name=a;
    formula=b;
    short_name=shortn;
    this->type=type;
    input_num=0;
    variable.clear();
    variable.shrink_to_fit();
    int i=0;
    while(i<name.size())
    {
        if(name[i]=='{')
        {
            i++;
            QString temp="";
            input_num++;
            while(i<name.size() && name[i]!='}')
            {
                temp=temp+QString(1,name[i]);
                i++;
            }
            variable.push_back(temp);
            b->var.push_back(temp);
        }
        i++;
    }
}

void Mod::set_formula_text(QString formula) {
    delete this->formula;
    this->formula = new Formula(formula);

    input_num=0;
    variable.clear();
    variable.shrink_to_fit();
    int i=0;
    while(i<name.size())
    {
        if(name[i]=='{')
        {
            i++;
            QString temp="";
            input_num++;
            while(i<name.size() && name[i]!='}')
            {
                temp=temp+QString(1,name[i]);
                i++;
            }
            variable.push_back(temp);
            this->formula->var.push_back(temp);
        }
        i++;
    }
}

std::vector<QString> Mod::get_labels(){
    return labels;
}

bool Mod::find_label(QString label){ //查找标签
    auto it=find(labels.begin(), labels.end(), label);
    if(it==labels.end())
        return false;
    else
        return true;
}
bool Mod::is_deleted(){
    return deleted;
}
bool Mod::search_str(QString aa){ //判断a是否在name中
    if(aa==QString(""))
        return true;
    return name.contains(aa);
}
QString Mod::get_name(){
    return name;
}
QString Mod::get_shortname(){
    return short_name;
}
Mod::Mod(int mod_id, QString a, Formula *b, enum RECORD_TYPE type , QString shortn){ //一个id，一个 使用手机 {} 分钟，使用电脑 {} 分钟 ，一个公式,一个类型
    name=a;
    formula=b;
    id=mod_id;
    short_name=shortn;
    this->type=type;
    deleted=false;
    input_num=0;
    int i=0;
    while(i<name.size())
    {
        if(name[i]=='{')
        {
            i++;
            QString temp="";
            input_num++;
            while(i<name.size() && name[i]!='}')
            {
                temp=temp+QString(1,name[i]);
                i++;
            }
            variable.push_back(temp);
            b->var.push_back(temp);
        }
        i++;
    }
}



bool Mod::name_legal(){
    int cntt=0;
    for(int i=0;i<name.size();i++)
    {
        if(name[i]=='{')
            cntt++;
        if(name[i]=='}')
            cntt--;
        if(cntt<0)
            return false;
        if(cntt>=2)
            return false;
    }
    if(cntt!=0)
        return false;
    for(int i=0;i<input_num;i++)
    {
        if(variable[i]==QString(""))
            return false;
        for(int j=i+1;j<input_num;j++)
        {
            if(variable[i]==variable[j])
                return false;
        }
    }
    return true;
}


