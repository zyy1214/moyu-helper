#include "focuswindow.h"
#include "./ui_focuswindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QMessageBox>
#include <QTimer>
#include <QDateTime>

FocusWindow::FocusWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FocusWindow)
{
    ui->setupUi(this);

    setPalette(QPalette(QColor(Qt::white)));
    // 设置窗口属性为无边框且始终保持最大化
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    showFullScreen();

    // 创建一个按钮用于关闭窗口
    ui->close->setText("close");
    connect(ui->close, &QPushButton::clicked, this, &QWidget::close);

    ui->nightButton->setText("night");
    connect(ui->nightButton,&QPushButton::clicked, [=](){
        isnight=1-isnight;
        if(isnight==0)
        {
            setPalette(QPalette(QColor(Qt::white)));
            ui->datelabel->setStyleSheet("font-size: 54px; color: #000000;");
            ui->timeLabel->setStyleSheet("font-size: 94px; color: #000000;");
            ui->elapsedTimeLabel->setStyleSheet("font-size: 54px; color: #000000;");
            ui->close->setStyleSheet("color: black;");
            ui->nightButton->setStyleSheet("color: black;");
        }
        else
        {
            setPalette(QPalette(QColor(Qt::black)));
            ui->datelabel->setStyleSheet("font-size: 54px; color: #ffffff;");
            ui->timeLabel->setStyleSheet("font-size: 94px; color: #ffffff;");
            ui->elapsedTimeLabel->setStyleSheet("font-size: 54px; color: #ffffff;");
            ui->close->setStyleSheet("color: black;");
            ui->nightButton->setStyleSheet("color: black;");
        }
    });

    // 记录当前时间作为窗口打开时的时间
    startTime = QDateTime::currentDateTime();


    // 设置时间标签的属性
    ui->datelabel->setAlignment(Qt::AlignCenter);
    ui->datelabel->setStyleSheet("font-size: 54px; color: #000000;");
    // 设置时间标签的属性
    ui->timeLabel->setAlignment(Qt::AlignCenter);
    ui->timeLabel->setStyleSheet("font-size: 94px; color: #000000;");
    // 设置运行时间标签的属性
    ui->elapsedTimeLabel->setAlignment(Qt::AlignCenter);
    ui->elapsedTimeLabel->setStyleSheet("font-size: 54px; color: #000000;");



    // 定时器每秒更新一次时间
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &FocusWindow::updateTime);
    timer->start(1000); // 每秒触发一次


    // 更新一次时间标签，以确保启动时显示当前时间
    updateTime();



    // // 创建一个中心部件并设置布局
    // QWidget *centralWidget = new QWidget(this);
    // centralWidget->setLayout(layout);
    // setCentralWidget(centralWidget);
}

FocusWindow::~FocusWindow()
{
    delete ui;
}

void FocusWindow::closeEvent(QCloseEvent *event)
{
    // 禁用窗口关闭事件，只有点击按钮时才能关闭
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Close",
                                                               "确定退出专注模式？",
                                                               QMessageBox::No | QMessageBox::Yes,
                                                               QMessageBox::No);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        event->accept();
    }
}
void FocusWindow::updateTime()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    //QString timeString = currentTime.toString("yyyy-MM-dd HH:mm");
    QString timeString = currentTime.toString("HH : mm");
    QString dateString = currentTime.toString("yyyy 年 MM 月 dd 日");
    ui->datelabel->setText(dateString);
    ui->timeLabel->setText(timeString);

    qint64 elapsedSeconds = startTime.secsTo(currentTime);
    int hours = elapsedSeconds / 3600;
    int minutes = (elapsedSeconds % 3600) / 60;
    int seconds = elapsedSeconds % 60;
    QString elapsedTimeString = QString("已专注时间: %1:%2:%3")
                                    .arg(hours, 2, 10, QChar('0'))
                                    .arg(minutes, 2, 10, QChar('0'))
                                    .arg(seconds, 2, 10, QChar('0'));
    ui->elapsedTimeLabel->setText(elapsedTimeString);
}
