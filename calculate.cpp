#include <bits/stdc++.h>
#include "calculate.h"
#include <QString>
#include <QChar>
using namespace std;
int t;
QString temp;
int priori(QString a)
{
    if(a=="!"||a=="`"||a=="~"||a=="abs"||a=="ceil"||a=="floor"||a=="sin"||a=="cos"||a=="sgn")//~为＋，`为-
        return 7;
    if(a=="^")
        return 6;
    if(a=="+")
        return 4;
    if(a=="-")
        return 4;
    if(a=="*")
        return 5;
    if(a=="/")
        return 5;
    if(a == "<" || a == ">" || a == ">=" || a == "<=" || a == "==" || a=="!=")
        return 3;
    if(a=="&&")
        return 2;
    if(a=="||")
        return 1;
    return 0;
}
bool isshu(QChar a)
{
    if(a.isDigit())
        return 1;
    if(a=='.')
        return 1;
    return 0;
}
bool check(QString a,QString b)
{
    if(priori(a)==7 && priori(b)==7)
        return false;
    return priori(a)<=priori(b);
}
bool equal_zero(double a)
{
    if(a>1e-7||a<-1e-7)
        return 0;
    return 1;
}
double cul(QString a,double b){
    if(a=="!")
    {
        if(b>1e-7 || b<-1e-7)
            return 0;
        else
            return 1;
    }
    if(a=="~")
    {
        return b;
    }
    if(a=="`")
        return -b;
    if(a=="abs")
        return abs(b);
    if(a=="floor")
        return floor(b);
    if(a=="ceil") {
        return ceil(b);
    }
    if(a=="sin")
        return sin(b);
    if(a=="cos")
        return cos(b);
    if(a=="sgn"){
        if(b>0)
            return 1;
        if(b==0)
            return 0;
        return -1;
    }
    return 0;
}
double cul(double a,QString b,double c)
{
    if(b=="+")
        return a+c;
    if(b=="-")
        return a-c;
    if(b=="*")
        return a*c;
    if(b=="/")
        return a/c;
    if(b=="<")
        return a<c;
    if(b==">")
        return a>c;
    if(b==">=")
        return a>=c;
    if(b=="<=")
        return a<=c;
    if(b=="==")
        return a==c;
    if(b=="!=")
        return a!=c;
    if(b=="^")
        return pow(a,c);
    if(b=="&&")
    {
        int x,y;
        if(a>1e-7 || a<-1e-7)
            x=0;
        else
            x=1;
        if(c>1e-7 || c<-1e-7)
            y=0;
        else
            y=1;
        return  double(x&y);
    }
    if(b=="||")
    {
        int x,y;
        if(a>1e-7 || a<-1e-7)
            x=0;
        else
            x=1;
        if(c>1e-7 || c<-1e-7)
            y=0;
        else
            y=1;
        return  double(x|y);
    }
    return 0;
}
bool check_pair(QString a)//判断是否是二元运算
{
    if(a!="!" && a!="~" && a!="`" && a!="abs" && a!="ceil" && a!= "floor" && a!="sin" && a!="cos" && a!="sgn")
        return true;
    return false;
}
bool is_alphabet(QChar a)
{
    if(a.isLower())
        return true;
    return false;
}
int calc(QString tempp)
{
    int n=0,cnt2=0,cnt1=0;//分别对应符号栈的栈顶以及数字栈的栈顶
    double s2[100]; //数字栈
    QChar a[500];
    QString s1[200]; // 符号栈
    for(int i=0;i<tempp.size();i++)
    {
        if(tempp[i]!=' ')
        {
            a[n]=tempp[i];
            n++;
        }
    }
    a[n+1]='\0';
    for(int i=0;i<n;)
    {
        if(a[i]=='i'&&a[i+1]=='f'&&a[i+2]=='(')
        {
            int j=i+3;
            int cnt=1; //括号对数,等于0时即为if的右括号
            int first_dots=0,second_dots=0;
            for(;j<n;j++)
            {
                if(a[j]=='(')
                    cnt++;
                if(a[j]==')')
                    cnt--;
                if(a[j]==',')
                {
                    if(first_dots==0&&cnt==1)
                        first_dots=j;
                    else if(second_dots==0&&cnt==1)
                        second_dots=j;
                }
                if(cnt==0)
                    break;
            }//此时j即为最后的)的位置，且第一个逗号与第二个逗号的位置已经求出
            QString str1,str2,str3; //分别对于三个参数的表达式
            double ans1,ans2,ans3;//分别对于三个参数的值
            str1.assign(a+i+3,a+first_dots);
            str2.assign(a+first_dots+1,a+second_dots);
            str3.assign(a+second_dots+1,a+j);
            //cout<<str1<<" "<<str2<<" "<<str3<<endl;
            ans1=calc(str1);
            ans2=calc(str2);
            ans3=calc(str3);
            //cout<<ans1<<" "<<ans2<<" "<<ans3<<endl;
            if(!equal_zero(ans1))
                s2[++cnt2]=ans2;
            else
                s2[++cnt2]=ans3;
            i=j+1;
        }
        else if(a[i]=='m'&&a[i+1]=='a'&&a[i+2]=='x'&&a[i+3]=='(')
        {
            int j=i+4;
            int cnt=1; //括号对数,等于0时即为if的右括号
            int dots_cnt=0,dots_pos[50];
            dots_pos[++dots_cnt]=j-1;
            for(;j<n;j++)
            {
                if(a[j]=='(')
                    cnt++;
                if(a[j]==')')
                    cnt--;
                if(a[j]==',')
                {
                    if(cnt==1)
                        dots_pos[++dots_cnt]=j;
                }
                if(cnt==0)
                    break;
            }//此时j即为最后的)的位置
            dots_pos[++dots_cnt]=j;
            QString str[50]; //分别对应每个参数的表达式
            double ans[50];//分别对应每个参数的值
            for(int i=1;i<dots_cnt;i++)
                str[i].assign(a+dots_pos[i]+1,a+dots_pos[i+1]);
            for(int i=1;i<dots_cnt;i++) {
                ans[i] = calc(str[i]);
                //cout<<dots_pos[i]<<" "<<ans[i]<<endl;
            }
            //cout<<ans1<<" "<<ans2<<" "<<ans3<<endl;
            double anss=-1e8;
            for(int i=1;i<dots_cnt;i++)
                anss=max(anss,ans[i]);
            s2[++cnt2]=anss;
            i=j+1;
        }
        else if(a[i]=='m'&&a[i+1]=='i'&&a[i+2]=='n'&&a[i+3]=='(')
        {
            int j=i+4;
            int cnt=1; //括号对数,等于0时即为if的右括号
            int dots_cnt=0,dots_pos[50];
            dots_pos[++dots_cnt]=j-1;
            for(;j<n;j++)
            {
                if(a[j]=='(')
                    cnt++;
                if(a[j]==')')
                    cnt--;
                if(a[j]==',')
                {
                    if(cnt==1)
                        dots_pos[++dots_cnt]=j;
                }
                if(cnt==0)
                    break;
            }//此时j即为最后的)的位置
            dots_pos[++dots_cnt]=j;
            QString str[50]; //分别对应每个参数的表达式
            double ans[50];//分别对应每个参数的值
            for(int i=1;i<dots_cnt;i++)
                str[i].assign(a+dots_pos[i]+1,a+dots_pos[i+1]);
            for(int i=1;i<dots_cnt;i++) {
                ans[i] = calc(str[i]);
                //cout<<dots_pos[i]<<" "<<ans[i]<<endl;
            }
            //cout<<ans1<<" "<<ans2<<" "<<ans3<<endl;
            double anss=1e8;
            for(int i=1;i<dots_cnt;i++)
                anss=min(anss,ans[i]);
            s2[++cnt2]=anss;
            i=j+1;
        }
        else if(a[i]=='(')
        {
            s1[++cnt1]=QString(1,a[i]);
            i++;
        }
        else if(a[i]==')')
        {
            while(s1[cnt1]!="(" && cnt1>=1)
            {
                if(check_pair(s1[cnt1]))
                {
                    double hh=cul(s2[cnt2-1],s1[cnt1],s2[cnt2]);
                    cnt1--;
                    cnt2--;
                    s2[cnt2]=hh;
                }
                else
                {
                    s2[cnt2]=cul(s1[cnt1],s2[cnt2]);
                    cnt1--;
                }
            }
            cnt1--;
            i++;
        }
        else if(isshu(a[i]))
        {
            QString tem="";
            while(isshu(a[i])&&i<n)
            {
                tem=tem+QString(1,a[i]);
                i++;
            }
            try {
                double x = tem.toDouble();
                s2[++cnt2]=x;
            }
            catch(invalid_argument& e){
                cerr<< "无效参数: " << e.what() << std::endl;
                break;
            }
        }
        else
        {
            if(i==0 || !isshu(a[i-1])&&a[i-1]!=')')
            {
                if(a[i]=='+')
                    a[i]='~';
                if(a[i]=='-')
                    a[i]='`';
            }
            QString temp=QString(1,a[i]);
            while(i+1<n && !isshu(a[i+1]) && a[i+1]!='(' && a[i+1]!='!' &&a[i+1]!='+'&&a[i+1]!='-' && (!is_alphabet(a[i+1])||
                                                                                                                                 is_alphabet(a[i]) ) )
            {
                temp=temp+QString(1,a[i+1]);
                i++;
            }
            while(cnt1>=1 && check(temp,s1[cnt1]))
            {
                if(check_pair(s1[cnt1]))
                {
                    double hh=cul(s2[cnt2-1],s1[cnt1],s2[cnt2]);
                    cnt1--;
                    cnt2--;
                    s2[cnt2]=hh;
                }
                else
                {
                    s2[cnt2]=cul(s1[cnt1],s2[cnt2]);
                    cnt1--;
                }
            }
            s1[++cnt1]=temp;
            i++;
        }
    }
    while(cnt1>=1)
    {
        if(check_pair(s1[cnt1]))
        {
            double hh=cul(s2[cnt2-1],s1[cnt1],s2[cnt2]);
            cnt1--;
            cnt2--;
            s2[cnt2]=hh;
        }
        else
        {
            s2[cnt2]=cul(s1[cnt1],s2[cnt2]);
            cnt1--;
        }
    }
    return int(s2[1]);
}
