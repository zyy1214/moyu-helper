#ifndef QTCHART_H
#define QTCHART_H
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QDate>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <Qtcharts/QStackedBarSeries>
#include <QDateTime>
#include <bits/stdc++.h>
#include "record.h"
#include "mod.h"
#include <QDebug>
QString placeholder[5]={""," ","  ","   ","    "};
QChartView* build_histogram_day(std::map<QDate, int> points_per_day){
    QBarSet *set0 = new QBarSet("积分/每天");
    int cnt=0;
    for(auto mr:points_per_day){
        *set0<<mr.second;
        cnt++;
    }
    if(cnt!=0){
        for(int i=cnt;i<5;i++){
            *set0<<0;
        }
    }
    QBarSeries *series = new QBarSeries();
    series->append(set0);
    QChart *chart = new QChart();
    chart->addSeries(series);
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    for(auto mr:points_per_day){
        axisX->append(mr.first.toString("dd MM yyyy"));
    }
    if(cnt!=0){
        for(int i=cnt;i<5;i++){
            axisX->append(placeholder[i]);
        }
    }
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    QChartView* chartView = new QChartView(chart);
    return chartView;
}
QChartView* build_histogram_month(std::map<QDate, int> points_per_day){
    QBarSet *set0 = new QBarSet("积分/每月");
    int temp_point=0;
    int temp_month=0,temp_year=0;
    int cnt=0;
    std::vector<QDate> y_date;
    if(!points_per_day.empty()){
        temp_month=points_per_day.begin()->first.month();
        temp_year=points_per_day.begin()->first.year();
        y_date.push_back(points_per_day.begin()->first);
        cnt++;
    }
    for(auto mr:points_per_day){
        if(mr.first.month()==temp_month&&mr.first.year()==temp_year){
            temp_point+=mr.second;
        }
        else{
            *set0<<temp_point;
            temp_point=mr.second;
            temp_year=mr.first.year();
            temp_month=mr.first.month();
            y_date.push_back(mr.first);
            cnt++;
        }

    }
    if(!y_date.empty()){
        *set0<<temp_point;
    }
    if(cnt!=0){
        for(int i=cnt;i<5;i++){
            *set0<<0;
        }
    }
    QBarSeries *series = new QBarSeries();
    series->append(set0);
    QChart *chart = new QChart();
    chart->addSeries(series);
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    for(auto mr:y_date){
        axisX->append(mr.toString("MM yyyy"));
    }
    if(cnt!=0){
        for(int i=cnt;i<5;i++){
            axisX->append(placeholder[i]);
        }
    }
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    QChartView* chartView = new QChartView(chart);
    return chartView;
}
QChartView* build_histogram_year(std::map<QDate, int> points_per_day){
    QBarSet *set0 = new QBarSet("积分/每年");
    int temp_point=0;
    int temp_year=0;
    std::vector<QDate> y_date;
    int cnt=0;
    if(!points_per_day.empty()){
        temp_year=points_per_day.begin()->first.year();
        y_date.push_back(points_per_day.begin()->first);
        cnt++;
    }
    for(auto mr:points_per_day){
        if(mr.first.year()==temp_year){
            temp_point+=mr.second;
        }
        else{
            *set0<<temp_point;
            temp_point=mr.second;
            temp_year=mr.first.year();
            y_date.push_back(mr.first);
            cnt++;
        }
    }
    if(!y_date.empty()){
        *set0<<temp_point;
    }
    if(cnt!=0){
        for(int i=cnt;i<5;i++){
            *set0<<0;
        }
    }
    QBarSeries *series = new QBarSeries();
    series->append(set0);
    QChart *chart = new QChart();
    chart->addSeries(series);
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    for(auto mr:y_date){
        axisX->append(mr.toString("yyyy"));
    }
    if(cnt!=0){
        for(int i=cnt;i<5;i++){
            axisX->append(placeholder[i]);
        }
    }
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    QChartView* chartView = new QChartView(chart);
    return chartView;
}
QChartView* build_histogram_day_all(std::map<QDate, int> points_per_day){
    QStackedBarSeries *series=new QStackedBarSeries();
    QBarSet *set0 = new QBarSet("获取积分/每天");
    QBarSet *set1 = new QBarSet("消耗积分/每天");
    int cnt=0;
    for(auto mr:points_per_day){
        if(mr.second>=0){
            *set0<<mr.second;
            *set1<<0;
            cnt++;
        }
        else{
            *set0<<0;
            *set1<<-mr.second;
            cnt++;
        }
    }
    if(cnt!=0){
        for(int i=cnt;i<5;i++){
            *set0<<0;
            *set1<<0;
        }
    }
    series->append(set0);
    series->append(set1);
    QChart *chart = new QChart();
    chart->addSeries(series);
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    for(auto mr:points_per_day){
        axisX->append(mr.first.toString("dd MM yyyy"));
    }
    if(cnt!=0){
        for(int i=cnt;i<5;i++){
            axisX->append(placeholder[i]);
        }
    }
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    QChartView* chartView = new QChartView(chart);
    return chartView;
}
QChartView* build_histogram_month_all(std::map<QDate, int> points_per_day){
    QStackedBarSeries *series=new QStackedBarSeries();
    QBarSet *set0 = new QBarSet("获取积分/每天");
    QBarSet *set1 = new QBarSet("消耗积分/每天");
    int temp_point=0;
    int temp_month=0,temp_year=0;
    int cnt=0;
    std::vector<QDate> y_date;
    if(!points_per_day.empty()){
        temp_month=points_per_day.begin()->first.month();
        temp_year=points_per_day.begin()->first.year();
        y_date.push_back(points_per_day.begin()->first);
        cnt++;
    }
    for(auto mr:points_per_day){
        if(mr.first.month()==temp_month&&mr.first.year()==temp_year){
            temp_point+=mr.second;
        }
        else{
            if(temp_point>=0){
                *set0<<temp_point;
                *set1<<0;
            }
            else{
                *set0<<0;
                *set1<<-temp_point;
            }
            temp_point=mr.second;
            temp_year=mr.first.year();
            temp_month=mr.first.month();
            y_date.push_back(mr.first);
            cnt++;
        }

    }
    if(!y_date.empty()){
        if(temp_point>=0){
            *set0<<temp_point;
            *set1<<0;
        }
        else{
            *set0<<0;
            *set1<<-temp_point;
        }
    }
    if(cnt!=0){
        for(int i=cnt;i<5;i++){
            *set0<<0;
            *set1<<0;
        }
    }
    series->append(set0);
    series->append(set1);
    QChart *chart = new QChart();
    chart->addSeries(series);
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    for(auto mr:y_date){
        axisX->append(mr.toString("MM yyyy"));
    }
    if(cnt!=0){
        for(int i=cnt;i<5;i++){
            axisX->append(placeholder[i]);
        }
    }
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    QChartView* chartView = new QChartView(chart);
    return chartView;
}
QChartView* build_histogram_year_all(std::map<QDate, int> points_per_day){
    QStackedBarSeries *series=new QStackedBarSeries();
    QBarSet *set0 = new QBarSet("获得积分/每天");
    QBarSet *set1 = new QBarSet("消耗积分/每天");
    int temp_point=0;
    int temp_year=0;
    int cnt=0;
    std::vector<QDate> y_date;
    if(!points_per_day.empty()){
        temp_year=points_per_day.begin()->first.year();
        y_date.push_back(points_per_day.begin()->first);
        cnt++;
    }
    for(auto mr:points_per_day){
        if(mr.first.year()==temp_year){
            temp_point+=mr.second;
        }
        else{
            if(temp_point>=0){
                *set0<<temp_point;
                *set1<<0;
            }
            else{
                *set0<<0;
                *set1<<-temp_point;
            }
            temp_point=mr.second;
            temp_year=mr.first.year();
            y_date.push_back(mr.first);
            cnt++;
        }
    }
    if(!y_date.empty()){
        if(temp_point>=0){
            *set0<<temp_point;
            *set1<<0;
        }
        else{
            *set0<<0;
            *set1<<-temp_point;
        }
    }
    if(cnt!=0){
        for(int i=cnt;i<5;i++){
            *set0<<0;
            *set1<<0;
        }
    }
    series->append(set0);
    series->append(set1);
    QChart *chart = new QChart();
    chart->addSeries(series);
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    for(auto mr:y_date){
        axisX->append(mr.toString("yyyy"));
    }
    if(cnt!=0){
        for(int i=cnt;i<5;i++){
            axisX->append(placeholder[i]);
        }
    }
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    QChartView* chartView = new QChartView(chart);
    return chartView;
}
QChartView* build_histogram(std::map<QDate, int> points_per_day,QString time,QString type){
    if(type!="全部"){
        if(time=="天")
            return build_histogram_day(points_per_day);
        else if(time=="月")
            return build_histogram_month(points_per_day);
        else
            return build_histogram_year(points_per_day);
    }
    if(time=="天")
        return build_histogram_day_all(points_per_day);
    else if(time=="月")
        return build_histogram_month_all(points_per_day);
    else
        return build_histogram_year_all(points_per_day);
}
QChartView* build_piechart(std::map<Mod*,int> points_per_mod,int else_point,bool have_else){
    QPieSeries *series = new QPieSeries();
    for(auto mr:points_per_mod){
        *series<<new QPieSlice(mr.first->get_shortname(),mr.second);
    }
    if(have_else)
        *series<<new QPieSlice("其它",else_point);
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("积分/模板");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    QChartView *chartView = new QChartView(chart);
    return chartView;
}
QChartView* build_linechart_day(std::map<QDate, int> points_per_day){
    QLineSeries *series = new QLineSeries();
    int tmp=0;
    int cnt=0;
    QDate temp_date;
    for(auto mr:points_per_day){
        cnt++;
        temp_date=mr.first;
        auto it=points_per_day.find(temp_date.addDays(-1));
        if(cnt!=1&&it==points_per_day.end()){
            series->append(QDateTime(temp_date.addDays(-1),QTime(0,0)).toMSecsSinceEpoch(),tmp);
        }
        tmp+=mr.second;
        series->append(QDateTime(temp_date,QTime(0,0)).toMSecsSinceEpoch(),tmp);
    }
    if(cnt==1){
        series->append(QDateTime(temp_date.addDays(-1),QTime(0,0)).toMSecsSinceEpoch(),0);
    }
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("积分/每天");
    if(cnt!=0){
        QDateTimeAxis *axisX = new QDateTimeAxis;
        axisX->setFormat("dd MM yyyy");
        axisX->setTitleText("日期");
        QValueAxis *axisY = new QValueAxis;
        axisY->setTitleText("积分");
        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }
    QChartView *chartView = new QChartView(chart);
    return chartView;
}
QChartView* build_linechart_month(std::map<QDate, int> points_per_day){
    QLineSeries *series = new QLineSeries();
    int temp_point=0;
    int temp_month=0,temp_year=0;
    int tmp=0;
    int cnt=0;
    QDate temp_date;
    std::vector<QDate> x_date;
    if(!points_per_day.empty()){
        temp_month=points_per_day.begin()->first.month();
        temp_year=points_per_day.begin()->first.year();
        QDate the_date=points_per_day.begin()->first;
        temp_date=QDate(the_date.year(),the_date.month(),1);
        x_date.push_back(temp_date);
    }
    for(auto mr:points_per_day){
        if(mr.first.month()==temp_month&&mr.first.year()==temp_year){
            temp_point+=mr.second;
        }
        else{
            cnt++;
            if(cnt!=1){
                series->append(QDateTime(temp_date.addDays(-1),QTime(0,0)).toMSecsSinceEpoch(),tmp);
            }
            tmp+=temp_point;
            series->append(QDateTime(temp_date,QTime(0,0)).toMSecsSinceEpoch(),tmp);
            temp_point=mr.second;
            temp_year=mr.first.year();
            temp_month=mr.first.month();
            temp_date=QDate(temp_year,temp_month,1);
            x_date.push_back(temp_date);
        }

    }
    if(!points_per_day.empty()){
        cnt++;
        if(cnt!=1){
            series->append(QDateTime(temp_date.addDays(-1),QTime(0,0)).toMSecsSinceEpoch(),tmp);
        }
        tmp+=temp_point;
        series->append(QDateTime(temp_date,QTime(0,0)).toMSecsSinceEpoch(),tmp);
    }
    if(cnt==1){
        series->append(QDateTime(temp_date.addDays(-1),QTime(0,0)).toMSecsSinceEpoch(),0);
        series->append(QDateTime(temp_date.addMonths(-1),QTime(0,0)).toMSecsSinceEpoch(),0);
    }
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("积分/每月");
    if(cnt!=0){
        QDateTimeAxis *axisX = new QDateTimeAxis;
        axisX->setFormat("MM yyyy");
        axisX->setTitleText("日期");
        QValueAxis *axisY = new QValueAxis;
        axisY->setTitleText("积分");
        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }
    QChartView *chartView = new QChartView(chart);
    return chartView;
}
QChartView* build_linechart_year(std::map<QDate, int> points_per_day){
    QLineSeries *series = new QLineSeries();
    int temp_point=0;
    int temp_year=0;
    int tmp=0;
    int cnt=0;
    QDate temp_date;
    std::vector<QDate> x_date;
    if(!points_per_day.empty()){
        temp_year=points_per_day.begin()->first.year();
        QDate the_date=points_per_day.begin()->first;
        temp_date=QDate(the_date.year(),1,1);
        x_date.push_back(temp_date);
    }
    for(auto mr:points_per_day){
        if(mr.first.year()==temp_year){
            temp_point+=mr.second;
        }
        else{
            cnt++;
            if(cnt!=1){
                series->append(QDateTime(temp_date.addDays(-1),QTime(0,0)).toMSecsSinceEpoch(),tmp);
            }
            tmp+=temp_point;
            series->append(QDateTime(temp_date,QTime(0,0)).toMSecsSinceEpoch(),tmp);
            temp_point=mr.second;
            temp_year=mr.first.year();
            temp_date=QDate(temp_year,1,1);
            x_date.push_back(temp_date);
        }
    }
    if(!points_per_day.empty()){
        cnt++;
        if(cnt!=1){
            series->append(QDateTime(temp_date.addDays(-1),QTime(0,0)).toMSecsSinceEpoch(),tmp);
        }
        tmp+=temp_point;
        series->append(QDateTime(temp_date,QTime(0,0)).toMSecsSinceEpoch(),tmp);
    }
    if(cnt==1){
        series->append(QDateTime(temp_date.addDays(-1),QTime(0,0)).toMSecsSinceEpoch(),0);
        series->append(QDateTime(temp_date.addYears(-1),QTime(0,0)).toMSecsSinceEpoch(),0);
    }
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("积分/每年");
    if(cnt!=0){
        QDateTimeAxis *axisX = new QDateTimeAxis;
        axisX->setFormat("yyyy");
        axisX->setTitleText("日期");
        QValueAxis *axisY = new QValueAxis;
        axisY->setTitleText("积分");
        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }
    QChartView *chartView = new QChartView(chart);
    return chartView;
}
QChartView* build_linechart(std::map<QDate, int> points_per_day,QString time){
    if(time=="天")
        return build_linechart_day(points_per_day);
    else if(time=="月")
        return build_linechart_month(points_per_day);
    else
        return build_linechart_year(points_per_day);
}
#endif // QTCHART_H
