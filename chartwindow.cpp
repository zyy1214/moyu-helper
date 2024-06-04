#include "chartwindow.h"
#include "ui_chartwindow.h"
#include "record.h"
#include <QApplication>
#include <QComboBox>
#include <QVBoxLayout>
#include <QWidget>
#include "qtchart.h"
#include <QDebug>
int total_points = 0;
int last_week_points = 0;
std::map<QString,std::list<Mod *> > labels_to_mods;
void ChartWindow::build_map(){
    labels_to_mods.clear();
    for(int i = 0;i < data->mods.size(); i++){
        std::vector<QString> temp_lables = data->mods[i]->get_labels();
        for(int j = 0;j < temp_lables.size(); j++){
            labels_to_mods[temp_lables[j]].push_back(data->mods[i]);
        }
    }
}
ChartWindow::ChartWindow(Data *data, QWidget *parent)
    : data(data), QMainWindow(parent)
    , ui(new Ui::ChartWindow)
{
    ui->setupUi(this);
    ui->date_end->setDate(QDate::currentDate());
    build_map();
    for(auto i = labels_to_mods.begin();i != labels_to_mods.end(); i++){
        ui->select_label->addItem(i->first);
    }
    connect(data, &Data::label_modified, this, &ChartWindow::modify_label);
}
void ChartWindow::modify_label(){
    build_map();
    int item_count=ui->select_label->count();
    while(item_count > 2) {
        ui->select_label->removeItem(item_count - 1);
        item_count=ui->select_label->count();
    }
    for(auto i = labels_to_mods.begin();i != labels_to_mods.end(); i++){
        ui->select_label->addItem(i->first);
    }
}
bool ChartWindow::will_build(){
    if(ui->select_label->currentText()=="-"||ui->select_label->currentText()=="")
        return 0;
    if(ui->select_mod->currentText()=="-"||ui->select_mod->currentText()=="")
        return 0;
    if(ui->select_graph->currentText()=="-"||ui->select_graph->currentText()=="")
        return 0;
    if(ui->select_graph->currentText()!="饼状图"&&(ui->select_time->currentText()=="-"||ui->select_time->currentText()==""))
        return 0;
    return 1;
}
std::map<QDate,int> ChartWindow::get_point_per_day(){
    QDate start_day=ui->date_start->date();
    QDate end_day=ui->date_end->date();
    std::map<QDate,int> point_per_day;
    if(ui->select_mod->currentText()!="全部"){
        for(auto mr: data->records){
            if(mr.first>=start_day&&mr.first<=end_day){
                int temp_point=0;
                bool is_changed=0;
                for (auto r : *(mr.second)) {
                    if(r->get_class() != BY_MOD)
                        continue;
                    if(((RecordByMod*)r)->get_mod()->get_shortname()!=ui->select_mod->currentText())
                        continue;
                    if(ui->select_type->currentText()=="全部"){
                        temp_point+=r->get_signed_point();
                        is_changed=1;
                    }
                    else if(ui->select_type->currentText()=="获取"){
                        if (r->get_type() != OBTAIN)
                            continue;
                        temp_point+=r->get_point();

                        is_changed=1;
                    }
                    else{
                        if(r->get_type()!=CONSUME)
                            continue;
                        temp_point+=r->get_point();
                        is_changed=1;
                    }
                }
                if(is_changed)
                    point_per_day[mr.first]=temp_point;
            }
        }
        return point_per_day;
    }
    if(ui->select_label->currentText()=="全部"){
        for(auto mr: data->records){
            if(mr.first>=start_day&&mr.first<=end_day){
                int temp_point=0;
                bool is_changed=0;
                for (auto r : *(mr.second)) {
                    if(ui->select_type->currentText()=="全部"){
                        temp_point+=r->get_signed_point();
                        is_changed=1;
                    }
                    else if(ui->select_type->currentText()=="获取"){
                        if (r->get_type() != OBTAIN)
                            continue;
                        temp_point+=r->get_point();
                        is_changed=1;
                    }
                    else{
                        if(r->get_type()!=CONSUME)
                            continue;
                        temp_point+=r->get_point();
                        is_changed=1;
                    }
                }
                if(is_changed)
                    point_per_day[mr.first]=temp_point;
            }
        }
        return point_per_day;
    }
    for(auto mr: data->records){
        if(mr.first>=start_day&&mr.first<=end_day){
            int temp_point=0;
            bool is_changed=0;
            for (auto r : *(mr.second)) {
                if(r->get_class() != BY_MOD)
                    continue;
                std::vector<QString> temp_lables=((RecordByMod*)r)->get_mod()->get_labels();
                if(std::find(temp_lables.begin(),temp_lables.end(),ui->select_label->currentText())==temp_lables.end())
                    continue;
                if(ui->select_type->currentText()=="全部"){
                    temp_point+=r->get_signed_point();
                    is_changed=1;
                }
                else if(ui->select_type->currentText()=="获取"){
                    if (r->get_type() != OBTAIN)
                        continue;
                    temp_point+=r->get_point();
                    is_changed=1;
                }
                else{
                    if(r->get_type()!=CONSUME)
                        continue;
                    temp_point+=r->get_point();
                    is_changed=1;
                }
            }
            if(is_changed)
                point_per_day[mr.first]=temp_point;
        }
    }
    return point_per_day;
}
int else_point;
bool have_else;
std::map<Mod*,int> ChartWindow::get_point_per_mod(){
    QDate start_day=ui->date_start->date();
    QDate end_day=ui->date_end->date();
    std::map<Mod*,int> point_per_mod;
    have_else=0;
    else_point=0;
    for(auto mr:data->records){
        if(mr.first>=start_day&&mr.first<=end_day){
            for (auto r : *(mr.second)) {
                if(r->get_class() != BY_MOD){
                    if(ui->select_type->currentText()=="获取"){
                        if (r->get_type() != OBTAIN)
                            continue;
                        else_point+=r->get_point();
                        have_else=1;
                    }
                    else{
                        if(r->get_type()!=CONSUME)
                            continue;
                        else_point+=r->get_point();
                        have_else=1;
                    }
                    continue;
                }
                std::vector<QString> temp_lables=((RecordByMod*)r)->get_mod()->get_labels();
                if(ui->select_label->currentText()=="全部"){
                    if(ui->select_type->currentText()=="获取"){
                        if (r->get_type() != OBTAIN)
                            continue;
                        if(point_per_mod.find(((RecordByMod*)r)->get_mod())==point_per_mod.end())
                            point_per_mod[((RecordByMod*)r)->get_mod()]=0;
                        point_per_mod[((RecordByMod*)r)->get_mod()]+=r->get_point();
                    }
                    else{
                        if(r->get_type()!=CONSUME)
                            continue;
                        if(point_per_mod.find(((RecordByMod*)r)->get_mod())==point_per_mod.end())
                            point_per_mod[((RecordByMod*)r)->get_mod()]=0;
                        point_per_mod[((RecordByMod*)r)->get_mod()]+=r->get_point();
                    }
                    continue;
                }
                if(std::find(temp_lables.begin(),temp_lables.end(),ui->select_label->currentText())==temp_lables.end())
                    continue;
                if(ui->select_type->currentText()=="获取"){
                    if (r->get_type() != OBTAIN)
                        continue;
                    if(point_per_mod.find(((RecordByMod*)r)->get_mod())==point_per_mod.end())
                        point_per_mod[((RecordByMod*)r)->get_mod()]=0;
                    point_per_mod[((RecordByMod*)r)->get_mod()]+=r->get_point();
                }
                else{
                    if(r->get_type()!=CONSUME)
                        continue;
                    if(point_per_mod.find(((RecordByMod*)r)->get_mod())==point_per_mod.end())
                        point_per_mod[((RecordByMod*)r)->get_mod()]=0;
                    point_per_mod[((RecordByMod*)r)->get_mod()]+=r->get_point();
                }
            }
        }
    }
    return point_per_mod;
}
ChartWindow::~ChartWindow()
{
    delete ui;
}

void ChartWindow::on_select_label_currentIndexChanged(int index)
{
    QString selected_command = ui->select_label->itemText(index);
    ui->select_mod->clear();
    ui->select_mod->addItem("-");
    if(selected_command!="-")
        ui->select_mod->addItem("全部");
    if(selected_command=="全部"){
        for(int i = 0;i < data->mods.size(); i++){
            ui->select_mod->addItem(data->mods[i]->get_shortname());
        }
    }
    else{
        for(auto i = labels_to_mods[selected_command].begin();i != labels_to_mods[selected_command].end(); i++){
            ui->select_mod->addItem( (*i)->get_shortname());
        }
    }

}
void ChartWindow::build_graph(){
    if(!will_build())
        return;
    if(ui->select_graph->currentText()=="柱状图"){
        if(ui->chart_layout->itemAt(0)){
            QWidget *widget=ui->chart_layout->itemAt(0)->widget();
            ui->chart_layout->removeItem(ui->chart_layout->itemAt(0));
            delete qobject_cast<QChartView *>(widget);
        }
        std::map<QDate,int> point_per_day=get_point_per_day();
        QChartView* chartView=build_histogram(point_per_day,ui->select_time->currentText(),ui->select_type->currentText());
        chartView->setRenderHint(QPainter::Antialiasing);
        ui->chart_layout->addWidget(chartView);
    }
    if(ui->select_graph->currentText()=="饼状图"){
        if(ui->chart_layout->itemAt(0)){
            QWidget *widget=ui->chart_layout->itemAt(0)->widget();
            ui->chart_layout->removeItem(ui->chart_layout->itemAt(0));
            delete qobject_cast<QChartView *>(widget);
        }
        std::map<Mod*,int> point_per_mod=get_point_per_mod();
        QChartView* chartView;
        if(ui->select_label->currentText()=="全部")
            chartView=build_piechart(point_per_mod,else_point,have_else);
        else
            chartView=build_piechart(point_per_mod,else_point,0);
        chartView->setRenderHint(QPainter::Antialiasing);

        ui->chart_layout->addWidget(chartView);
    }
    if(ui->select_graph->currentText()=="折线图"){
        if(ui->chart_layout->itemAt(0)){
            QWidget *widget=ui->chart_layout->itemAt(0)->widget();
            ui->chart_layout->removeItem(ui->chart_layout->itemAt(0));
            delete qobject_cast<QChartView *>(widget);
        }
        std::map<QDate,int> point_per_day=get_point_per_day();
        QChartView* chartView=build_linechart(point_per_day,ui->select_time->currentText());
        chartView->setRenderHint(QPainter::Antialiasing);
        ui->chart_layout->addWidget(chartView);
    }
}

void ChartWindow::on_select_mod_currentIndexChanged(int index)
{
    QString selected_command = ui->select_mod->itemText(index);
    ui->select_graph->clear();
    ui->select_graph->addItem("-");
    if(selected_command!="-"){
        ui->select_graph->addItem("柱状图");
        ui->select_graph->addItem("折线图");
        if(selected_command!="全部"){
            ui->select_type->clear();
            for(auto mr:data->mods){
                if(mr->get_shortname()==selected_command){
                    if(mr->get_type()==OBTAIN)
                        ui->select_type->addItem("获取");
                    else
                        ui->select_type->addItem("消耗");
                }
            }

        }
    }
    if(selected_command=="全部"&&ui->select_type->currentText()!="全部"){
        ui->select_graph->addItem("饼状图");
    }
    if(selected_command=="-"||selected_command=="全部"){
        ui->select_type->clear();
        ui->select_type->addItem("全部");
        ui->select_type->addItem("获取");
        ui->select_type->addItem("消耗");
    }
    build_graph();
}

void ChartWindow::on_select_graph_currentIndexChanged(int index)
{
    QString selected_command = ui->select_graph->itemText(index);
    ui->select_time->clear();
    ui->select_time->addItem("-");
    if(selected_command=="柱状图"||selected_command=="折线图"){
        ui->select_time->addItem("天");
        ui->select_time->addItem("月");
        ui->select_time->addItem("年");
    }
    build_graph();
}


void ChartWindow::on_select_type_currentIndexChanged(int index)
{
    ui->select_graph->clear();
    ui->select_graph->addItem("-");
    if(ui->select_mod->currentText()!="-"){
        ui->select_graph->addItem("柱状图");
        ui->select_graph->addItem("折线图");
    }
    if(ui->select_mod->currentText()=="全部"&&ui->select_type->currentText()!="全部"){
        ui->select_graph->addItem("饼状图");
    }
    build_graph();
}


void ChartWindow::on_select_time_currentIndexChanged(int index)
{
    build_graph();
}


void ChartWindow::on_date_start_userDateChanged(const QDate &date)
{
    build_graph();
}


void ChartWindow::on_date_end_userDateChanged(const QDate &date)
{
    build_graph();
}

