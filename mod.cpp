#include <QString>
#include <QDebug>
#include <vector>
#include "mod.h"
int Mod::search_coun=0;
int Mod::temp_count=0;
int Mod::mod_cnt=0;
int Mod::temp_search[200] = {};
int Mod::mod_search[200]={};
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
}


void Mod::change(QString a, Formula *b, enum RECORD_TYPE typee){ //一个id，一个 使用手机 {} 分钟，使用电脑 {} 分钟 ，一个公式,一个类型
        name=a;
        formula=b;
        type=typee;
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
void Mod::add_label(QString label){ //加标签
    labels.push_back(label);
}
void Mod::delete_label(QString label){ //删除标签
    auto it=find(labels.begin(), labels.end(), label);
    if(it==labels.end())
        qDebug() << "未找到改标签";
    else
    {
        labels.erase(it);
    }
}
bool Mod::find_label(QString label){ //查找标签
    auto it=find(labels.begin(), labels.end(), label);
    if(it==labels.end())
        return false;
    else
        return true;
}
void Mod::delete_mod(){
    deleted=true;
}
bool Mod::isdelete(){
    return deleted;
}
bool Mod::search_str(QString aa){ //判断a是否在name中
    if(aa==QString(""))
        return true;
    return name.contains(aa);
}
QString Mod::print_name(){
    return name;
}
Mod::Mod(int mod_id, QString a, Formula *b, enum RECORD_TYPE typee){ //一个id，一个 使用手机 {} 分钟，使用电脑 {} 分钟 ，一个公式,一个类型
    name=a;
    formula=b;
    id=mod_id;
    type=typee;
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

std::vector<Mod*> mods; // 存放所有模板

void Mod::add_mod(QString a,Formula *b,enum RECORD_TYPE typee){//一个id，一个 使用手机 {} 分钟，使用电脑 {} 分钟 ，一个公式,一个类型
    Mod* aaa=new Mod(mod_cnt,a,b,typee);// id从0开始
    mod_cnt++;
    mods.push_back(aaa);
    mod_search[++search_coun]=mod_cnt-1;
}

void Mod::change_mod(int before_mod_id,QString a,Formula *b, enum RECORD_TYPE typee, enum RECORD_TYPE change_type){//更改模板，先加再删
    if(change_type==0) //0则为不更改，1为更改
    {
        Mod* aaa=new Mod(mod_cnt,a,b,typee);
        mod_cnt++;
        mods.push_back(aaa);
        mods[before_mod_id]->delete_mod();
        mod_search[++search_coun]=mod_cnt-1;
    }
    else
    {
        mods[before_mod_id]->change(a,b,typee);
/////////////////////////////  还要更改总积分，要看记录怎么写   /////////////////////////////

    }
}

void Mod::search_string(QString aa){//字符串搜索
    temp_count=0;
    for(int i=1;i<=search_coun;i++)
    {
        if(mods[mod_search[i]]->isdelete()==true)//如果是被删除的模板就跳过
            continue;
        if(mods[mod_search[i]]->search_str(aa)==true)
        {
            temp_search[++temp_count]=mod_search[i];//如果找到一个符合要求的模板就加进temp里
        }
    }
    search_coun=temp_count;
    for(int i=1;i<=temp_count;i++)
        mod_search[i]=temp_search[i];
}

void Mod::search_tag(QString aa){
    temp_count=0;
    for(int i=1;i<=search_coun;i++)
    {
        if(mods[mod_search[i]]->isdelete()==true)//如果是被删除的模板就跳过
            continue;
        if(mods[mod_search[i]]->find_label(aa)==true)
        {
            temp_search[++temp_count]=mod_search[i];//如果找到一个符合要求的模板就加进temp里
        }
    }
    search_coun=temp_count;
    for(int i=1;i<=temp_count;i++)
        mod_search[i]=temp_search[i];
}

void Mod::search(QString a, std::vector<QString> b){ //a是查询的字符串，b是标签的数组
    search_coun=mods.size();
    for(int i=1;i<=search_coun;i++)
        mod_search[i]=i-1;
    search_string(a);
    for(int i=0;i<b.size();i++)
        search_tag(b[i]);
    if(search_coun==0)
        qDebug() << "未搜索到结果";
    else
    {
        for(int i=1;i<=search_coun;i++)
        {
            mods[mod_search[i]]->print_name();
/////////////////////////////  要结合ui   /////////////////////////////
        }
    }
}

bool Mod::right_name(){
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


