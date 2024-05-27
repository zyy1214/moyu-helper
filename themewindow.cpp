#include "themewindow.h"
#include "ui_themewindow.h"

ThemeWindow::ThemeWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ThemeWindow)
{
    ui->setupUi(this);
}

ThemeWindow::~ThemeWindow()
{
    delete ui;
}
