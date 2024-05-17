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
QChartView* build_histogram_day(std::map<QDate, int> points_per_day){
    QBarSet *set0 = new QBarSet("Points Per Day");
    for(auto mr:points_per_day){
        *set0<<mr.second;
    }
    QBarSeries *series = new QBarSeries();
    series->append(set0);
    QChart *chart = new QChart();
    chart->addSeries(series);
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    for(auto mr:points_per_day){
        axisX->append(mr.first.toString("dd MM yyyy"));
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
    QBarSet *set0 = new QBarSet("Points Per Month");
    int temp_point=0;
    int temp_month=0,temp_year=0;
    std::vector<QDate> y_date;
    if(!points_per_day.empty()){
        temp_month=points_per_day.begin()->first.month();
        temp_year=points_per_day.begin()->first.year();
        y_date.push_back(points_per_day.begin()->first);
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
        }

    }
    if(!y_date.empty()){
        *set0<<temp_point;
    }
    QBarSeries *series = new QBarSeries();
    series->append(set0);
    QChart *chart = new QChart();
    chart->addSeries(series);
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    for(auto mr:y_date){
        axisX->append(mr.toString("MM yyyy"));
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
    QBarSet *set0 = new QBarSet("Points Per Year");
    int temp_point=0;
    int temp_year=0;
    std::vector<QDate> y_date;
    if(!points_per_day.empty()){
        temp_year=points_per_day.begin()->first.year();
        y_date.push_back(points_per_day.begin()->first);
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
        }
    }
    if(!y_date.empty()){
        *set0<<temp_point;
    }
    QBarSeries *series = new QBarSeries();
    series->append(set0);
    QChart *chart = new QChart();
    chart->addSeries(series);
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    for(auto mr:y_date){
        axisX->append(mr.toString("yyyy"));
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
    QBarSet *set0 = new QBarSet("Obtain Points Per Day");
    QBarSet *set1 = new QBarSet("Consume Points Per Day");
    for(auto mr:points_per_day){
        if(mr.second>=0){
            *set0<<mr.second;
            *set1<<0;
        }
        else{
            *set0<<0;
            *set1<<-mr.second;
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
    QBarSet *set0 = new QBarSet("Obtain Points Per Month");
    QBarSet *set1 = new QBarSet("Consume Points Per Month");
    int temp_point=0;
    int temp_month=0,temp_year=0;
    std::vector<QDate> y_date;
    if(!points_per_day.empty()){
        temp_month=points_per_day.begin()->first.month();
        temp_year=points_per_day.begin()->first.year();
        y_date.push_back(points_per_day.begin()->first);
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
    series->append(set0);
    series->append(set1);
    QChart *chart = new QChart();
    chart->addSeries(series);
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    for(auto mr:y_date){
        axisX->append(mr.toString("MM yyyy"));
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
    QBarSet *set0 = new QBarSet("Obtain Points Per Year");
    QBarSet *set1 = new QBarSet("Consume Points Per Year");
    int temp_point=0;
    int temp_year=0;
    std::vector<QDate> y_date;
    if(!points_per_day.empty()){
        temp_year=points_per_day.begin()->first.year();
        y_date.push_back(points_per_day.begin()->first);
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
    series->append(set0);
    series->append(set1);
    QChart *chart = new QChart();
    chart->addSeries(series);
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    for(auto mr:y_date){
        axisX->append(mr.toString("yyyy"));
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
    if(type!="All"){
        if(time=="Day")
            return build_histogram_day(points_per_day);
        else if(time=="Month")
            return build_histogram_month(points_per_day);
        else
            return build_histogram_year(points_per_day);
    }
    if(time=="Day")
        return build_histogram_day_all(points_per_day);
    else if(time=="Month")
        return build_histogram_month_all(points_per_day);
    else
        return build_histogram_year_all(points_per_day);
}
QChartView* build_piechart(std::map<Mod*,int> points_per_mod,int else_point,bool have_else){
    QPieSeries *series = new QPieSeries();
    for(auto mr:points_per_mod){
        *series<<new QPieSlice(mr.first->print_name(),mr.second);
    }
    if(have_else)
        *series<<new QPieSlice("其它",else_point);
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Points Per Mod");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    QChartView *chartView = new QChartView(chart);
    return chartView;
}
#endif // QTCHART_H
