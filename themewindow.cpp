#include "themewindow.h"
#include "ui_themewindow.h"
#include "colorprovider.h"
#include "data_storage.h"
#include <QPushButton>
#include <QDebug>
#include <QFile>
#include <QLayoutItem>
#include <QLayout>
#include <QFont>
ThemeWindow::ThemeWindow(QMainWindow* mainwindow, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ThemeWindow),mainwindow(mainwindow)
{
    ui->setupUi(this);
    mainwindow->setPalette(QPalette(QColor(Qt::white)));
    ui->themebutton1->setStyleSheet("QPushButton {"
                                    "border-image: url(:/images/theme) 0 0 0 0 stretch stretch;"
                                    "}");
    ui->themebutton2->setStyleSheet("QPushButton {"
                                    "border-image: url(:/images/theme2) 0 0 0 0 stretch stretch;"
                                    "}");
    ui->themebutton3->setStyleSheet("QPushButton {"
                                    "border-image: url(:/images/theme3) 0 0 0 0 stretch stretch;"
                                    "}");

    if(get_value("backgroundpic")=="0"){
        on_themebutton4_clicked();
    }
    else if(get_value("backgroundpic")=="1"){
        on_themebutton1_clicked();
    }
    else if(get_value("backgroundpic")=="2"){
        on_themebutton2_clicked();
    }
    else if(get_value("backgroundpic")=="3"){
        on_themebutton3_clicked();
    }

    if (get_value("dark_mode") == "1") {
        ui->checkBox->setCheckState(Qt::Checked);
        on_checkBox_stateChanged(0);
    }
}

ThemeWindow::~ThemeWindow()
{
    delete ui;
}

void ThemeWindow::on_themebutton1_clicked()
{
    // mainwindow->setStyleSheet("QMainWindow {"
    //                     "background-image: url(:/images/theme);"
    //                     "background-repeat: no-repeat;"
    //                     "background-position: center;"
    //                     "}");
    save_value("backgroundpic","1");
    mainwindow->repaint();
}


void ThemeWindow::on_themebutton2_clicked()
{
    // mainwindow->setStyleSheet("QMainWindow {"
    //                          "background-image: url(:/images/theme2);"
    //                          "background-repeat: no-repeat;"
    //                          "background-position: center;"
    //                          "}");
    save_value("backgroundpic","2");
    mainwindow->repaint();
}


void ThemeWindow::on_themebutton3_clicked()
{
    // mainwindow->setStyleSheet("QMainWindow {"
    //                          "background-image: url(:/images/theme3);"
    //                          "background-repeat: no-repeat;"
    //                          "background-position: center;"
    //                          "}");
    save_value("backgroundpic","3");
    mainwindow->repaint();
}


void ThemeWindow::on_themebutton4_clicked()
{
    // mainwindow->setStyleSheet("QMainWindow {"
    //                          "background-image: none;"
    //                          "background-color: white"
    //                          "}");
    save_value("backgroundpic","0");
    mainwindow->repaint();
}

void ThemeWindow::on_checkBox_stateChanged(int arg1) {
    dark_mode = (ui->checkBox->checkState() == Qt::Checked);
    ColorProvider::set_dark_mode(dark_mode);
    ui->checkBox->setStyleSheet("QCheckBox::indicator {width: 16px; height: 16px;} "
                                "QCheckBox {color:" + get_color("text_color").name() + ";}");
    if(dark_mode) {
        save_value("dark_mode", "1");
    } else {
        save_value("dark_mode", "0");
    }
}

