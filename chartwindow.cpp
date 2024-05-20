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
//std::map<QDate, MultipleRecord *, std::greater<QDate>> records;
//std::vector<Mod *> mods;

//Ui::ChartWindow *ui;
void init_data() {
    // mods.push_back(new Mod({"学习"},"背单词",RECORD_TYPE::OBTAIN));
    // mods.push_back(new Mod({"学习","运动"},"打篮球",RECORD_TYPE::OBTAIN));
    // mods.push_back(new Mod({"运动"},"睡觉", RECORD_TYPE::CONSUME));
    // mods.push_back(new Mod({"玩游戏"},"玩原神", RECORD_TYPE::CONSUME));
    // mods.push_back(new Mod({"运动","玩游戏"},"FIFA", RECORD_TYPE::CONSUME));
    // MultipleRecord *mr1 = new MultipleRecord();
    // QDate date{2024, 5, 3};
    // mr1->push_back(new RecordDirect("跑步5km", RECORD_TYPE::OBTAIN, 50, date));
    // mr1->push_back(new RecordDirect("练习引体向上 20 个", RECORD_TYPE::OBTAIN, 20, date));
    // mr1->push_back(new RecordDirect("摸鱼 1 小时", RECORD_TYPE::CONSUME, 25, date));
    // mr1->push_back(new RecordByMod(mods[0],239));
    // mr1->push_back(new RecordByMod(mods[4],40));
    // mr1->push_back(new RecordByMod(mods[2],21));
    // records[date] = mr1;
    // MultipleRecord *mr2 = new MultipleRecord;
    // date = {2024, 5, 1};
    // mr2->push_back(new RecordDirect("充值小月卡", RECORD_TYPE::CONSUME, 300, date));
    // mr2->push_back(new RecordDirect("跑步10km", RECORD_TYPE::OBTAIN, 100, date));
    // mr2->push_back(new RecordDirect("图书馆自习 2 小时", RECORD_TYPE::OBTAIN, 30, date));
    // mr2->push_back(new RecordByMod(mods[1],199));
    // mr2->push_back(new RecordByMod(mods[3],96));
    // records[date] = mr2;
    // MultipleRecord *mr3 = new MultipleRecord;
    // date = {2024, 4, 30};
    // mr3->push_back(new RecordDirect("买东西", RECORD_TYPE::CONSUME, 37, date));
    // mr3->push_back(new RecordDirect("跑步5km", RECORD_TYPE::OBTAIN, 50, date));
    // mr3->push_back(new RecordDirect("专注学习 3 小时", RECORD_TYPE::OBTAIN, 45, date));
    // mr3->push_back(new RecordDirect("刷谢慧民20道题", RECORD_TYPE::OBTAIN, 50, date));
    // mr3->push_back(new RecordByMod(mods[2],35));
    // mr3->push_back(new RecordByMod(mods[3],75));
    // records[{2024, 4, 30}] = mr3;
    // records[{2024, 4, 29}] = mr3;
    // records[{2024, 4, 28}] = mr3;
    // records[{2024, 4, 27}] = mr3;
    // records[{2024, 4, 26}] = mr3;
    // records[{2024, 4, 25}] = mr3;
    // for (auto mr : records) {
    //     for (auto r : *(mr.second)) {
    //         int point = r->get_signed_point();
    //         total_points += point;
    //         if (mr.first.daysTo(QDate::currentDate()) <= 6) {
    //             last_week_points += point;
    //         }
    //     }
    // }
}
std::map<QString,std::list<Mod *> > labels_to_mods;
void ChartWindow::build_map(){
    labels_to_mods.clear();
    for(int i = 0;i < mods.size(); i++){
        std::vector<QString> temp_lables = mods[i]->get_labels();
        for(int j = 0;j < temp_lables.size(); j++){
            labels_to_mods[temp_lables[j]].push_back(mods[i]);
        }
    }
}
ChartWindow::ChartWindow(Data *data, QWidget *parent)
    : data(data), QMainWindow(parent)
    , ui(new Ui::ChartWindow)
{
    init_data();
    ui->setupUi(this);
    ui->date_end->setDate(QDate::currentDate());
    build_map();
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
    if(ui->select_graph->currentText()!="Pie Chart"&&(ui->select_time->currentText()=="-"||ui->select_time->currentText()==""))
        return 0;
    return 1;
}
std::map<QDate,int> ChartWindow::get_point_per_day(){
    QDate start_day=ui->date_start->date();
    QDate end_day=ui->date_end->date();
    std::map<QDate,int> point_per_day;
    if(ui->select_mod->currentText()!="All"){
        for(auto mr: data->records){
            if(mr.first>=start_day&&mr.first<=end_day){
                int temp_point=0;
                bool is_changed=0;
                for (auto r : *(mr.second)) {
                    if(r->get_class() != BY_MOD)
                        continue;
                    if(((RecordByMod*)r)->get_mod()->get_name()!=ui->select_mod->currentText())
                        continue;
                    if(ui->select_type->currentText()=="All"){
                        temp_point+=r->get_signed_point();
                        is_changed=1;
                    }
                    else if(ui->select_type->currentText()=="Obtain"){
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
    if(ui->select_label->currentText()=="All"){
        for(auto mr: data->records){
            if(mr.first>=start_day&&mr.first<=end_day){
                int temp_point=0;
                bool is_changed=0;
                for (auto r : *(mr.second)) {
                    if(ui->select_type->currentText()=="All"){
                        temp_point+=r->get_signed_point();
                        is_changed=1;
                    }
                    else if(ui->select_type->currentText()=="Obtain"){
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
                if(ui->select_type->currentText()=="All"){
                    temp_point+=r->get_signed_point();
                    is_changed=1;
                }
                else if(ui->select_type->currentText()=="Obtain"){
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
                    if(ui->select_type->currentText()=="Obtain"){
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
                if(ui->select_label->currentText()=="All"){
                    if(ui->select_type->currentText()=="Obtain"){
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
                if(ui->select_type->currentText()=="Obtain"){
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
        ui->select_mod->addItem("All");
    if(selected_command=="All"){
        for(int i = 0;i < mods.size(); i++){
            ui->select_mod->addItem(mods[i]->get_name());
        }
    }
    else{
        for(auto i = labels_to_mods[selected_command].begin();i != labels_to_mods[selected_command].end(); i++){
            ui->select_mod->addItem( (*i)->get_name());
        }
    }

}
void ChartWindow::build_graph(){
    if(!will_build())
        return;
    if(ui->select_graph->currentText()=="Histogram"){
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
    if(ui->select_graph->currentText()=="Pie Chart"){
        if(ui->chart_layout->itemAt(0)){
            QWidget *widget=ui->chart_layout->itemAt(0)->widget();
            ui->chart_layout->removeItem(ui->chart_layout->itemAt(0));
            delete qobject_cast<QChartView *>(widget);
        }
        std::map<Mod*,int> point_per_mod=get_point_per_mod();
        QChartView* chartView;
        if(ui->select_label->currentText()=="All")
            chartView=build_piechart(point_per_mod,else_point,have_else);
        else
            chartView=build_piechart(point_per_mod,else_point,0);
        chartView->setRenderHint(QPainter::Antialiasing);

        ui->chart_layout->addWidget(chartView);
    }
    if(ui->select_graph->currentText()=="Line Chart"){
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
        ui->select_graph->addItem("Histogram");
        ui->select_graph->addItem("Line Chart");
        if(selected_command!="All"){
            ui->select_type->clear();
            for(auto mr:mods){
                if(mr->get_name()==selected_command){
                    if(mr->get_type()==OBTAIN)
                        ui->select_type->addItem("Obtain");
                    else
                        ui->select_type->addItem("Consume");
                }
            }

        }
    }
    if(selected_command=="All"&&ui->select_type->currentText()!="All"){
        ui->select_graph->addItem("Pie Chart");
    }
    if(selected_command=="-"||selected_command=="All"){
        ui->select_type->clear();
        ui->select_type->addItem("All");
        ui->select_type->addItem("Obtain");
        ui->select_type->addItem("Consume");
    }
    build_graph();
}

void ChartWindow::on_select_graph_currentIndexChanged(int index)
{
    QString selected_command = ui->select_graph->itemText(index);
    ui->select_time->clear();
    ui->select_time->addItem("-");
    if(selected_command=="Histogram"||selected_command=="Line Chart"){
        ui->select_time->addItem("Day");
        ui->select_time->addItem("Month");
        ui->select_time->addItem("Year");
    }
    build_graph();
}


void ChartWindow::on_select_type_currentIndexChanged(int index)
{
    ui->select_graph->clear();
    ui->select_graph->addItem("-");
    if(ui->select_mod->currentText()!="-"){
        ui->select_graph->addItem("Histogram");
        ui->select_graph->addItem("Line Chart");
    }
    if(ui->select_mod->currentText()=="All"&&ui->select_type->currentText()!="All"){
        ui->select_graph->addItem("Pie Chart");
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

