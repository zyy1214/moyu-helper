#include "themewindow.h"
#include "ui_themewindow.h"
#include "mainwindow.h"
#include "data_storage.h"
#include <QPushButton>
#include <QDebug>
#include <QFile>
ThemeWindow::ThemeWindow(QMainWindow* mainwindow,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ThemeWindow),mainwindow(mainwindow)
{
    ui->setupUi(this);
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
}

ThemeWindow::~ThemeWindow()
{
    delete ui;
}

void ThemeWindow::on_themebutton1_clicked()
{
    mainwindow->setStyleSheet("QMainWindow {"
                        "background-image: url(:/images/theme);"
                        "background-repeat: no-repeat;"
                        "background-position: center;"
                        "}");
    save_value("backgroundpic","1");
}


void ThemeWindow::on_themebutton2_clicked()
{
    mainwindow->setStyleSheet("QMainWindow {"
                             "background-image: url(:/images/theme2);"
                             "background-repeat: no-repeat;"
                             "background-position: center;"
                             "}");
    save_value("backgroundpic","2");
}


void ThemeWindow::on_themebutton3_clicked()
{
    mainwindow->setStyleSheet("QMainWindow {"
                             "background-image: url(:/images/theme3);"
                             "background-repeat: no-repeat;"
                             "background-position: center;"
                             "}");
    save_value("backgroundpic","3");
}


void ThemeWindow::on_themebutton4_clicked()
{
    mainwindow->setStyleSheet("QMainWindow {"
                             "background-image: none;"
                             "background-color: white"
                             "}");
    save_value("backgroundpic","0");
}

