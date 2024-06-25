#include "themewindow.h"
#include "ui_themewindow.h"
#include <QPushButton>
#include <QDebug>
#include <QFile>
ThemeWindow::ThemeWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ThemeWindow)
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
}

ThemeWindow::~ThemeWindow()
{
    delete ui;
}

void ThemeWindow::on_themebutton1_clicked()
{
    this->setStyleSheet("QMainWindow {"
                        "background-image: url(:/images/theme);"
                        "background-repeat: no-repeat;"
                        "background-position: center;"
                        "}");
}


void ThemeWindow::on_themebutton2_clicked()
{
    this->setStyleSheet("QMainWindow {"
                        "background-image: url(:/images/theme2);"
                        "background-repeat: no-repeat;"
                        "background-position: center;"
                        "}");
}


void ThemeWindow::on_themebutton3_clicked()
{
    this->setStyleSheet("QMainWindow {"
                        "background-image: url(:/images/theme3);"
                        "background-repeat: no-repeat;"
                        "background-position: center;"
                        "}");
}


void ThemeWindow::on_themebutton4_clicked()
{
    this->setStyleSheet("QMainWindow {"
                        "background-image: none;"
                        "}");
}

