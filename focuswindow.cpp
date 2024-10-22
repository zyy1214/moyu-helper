#include "focuswindow.h"
#include "./ui_focuswindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QMessageBox>
#include <QTimer>
#include <QDateTime>
#include <QPainter>
#include <Windows.h>
#include <wrl.h>
#include <shobjidl.h>
#include <ctime>
#include <unistd.h>
#include <cmath>

#include "network.h"
#include "uitools.h"


HHOOK g_hKeyboardHook = NULL;
HWND g_hWnd;
HWND hh1;
QTimer *checkTimer = nullptr; // 定义一个全局的定时器指针


// 钩子过程函数
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;
        switch (wParam) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            // 捕获 Alt+Tab、Win+D、Ctrl+Esc 等组合键
            if ((p->vkCode == VK_TAB && GetAsyncKeyState(VK_MENU) & 0x8000) || // Alt+Tab
                (p->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL) & 0x8000) || // Ctrl+Esc
                (p->vkCode == VK_LWIN || p->vkCode == VK_RWIN) || // Win key
                (p->vkCode == 0x44 && (GetAsyncKeyState(VK_LWIN) & 0x8000 || GetAsyncKeyState(VK_RWIN) & 0x8000))) { // Win+D
                return 1; // 阻止这些键的默认行为
            }
            return 1;
            break;
        }
        return 1;
    }
    return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

// 将窗口移到当前桌面并最大化
void bringToFrontAndMaximize() {
    // 将窗口带到前台
    SetForegroundWindow(g_hWnd);
    // 显示并最大化窗口
    //ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
}

// 检查并将窗口移到前台
void FocusWindow::checkAndBringToFront() {
    a.close();
    bringToFrontAndMaximize();
    showFullScreen();
    // 获取当前活动窗口的句柄
    hh1 = (HWND)a.winId();
    HWND foregroundWindow = GetForegroundWindow();
    if(foregroundWindow == hh1){
        a.close();
        bringToFrontAndMaximize();
        showFullScreen();
    }
    if (foregroundWindow != g_hWnd) {
        a.close();
        bringToFrontAndMaximize();
        showFullScreen();

        if (foregroundWindow != g_hWnd) {
            a.show();
            a.setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
            a.showFullScreen();
            bringToFrontAndMaximize();
            showFullScreen();
        }
    }
}

FocusWindow::FocusWindow(bool flag,int time,QWidget *parent)
    : QMainWindow(parent),flag(flag),countdowntime(time)
    , ui(new Ui::FocusWindow)
{
    ui->setupUi(this);

    if(flag==1)
    {
        canclose=0;
        ui->close->setEnabled(false);
    }
    else
        canclose=1;

    // 获取窗口句柄
    g_hWnd = (HWND)this->winId();

    // 定时器每100毫秒检查一次窗口位置
    checkTimer = new QTimer(this);
    connect(checkTimer, &QTimer::timeout, this, &FocusWindow::checkAndBringToFront);
    checkTimer->start(2000); // 每2000毫秒检查一次

    // 注册低级别键盘钩子，阻止桌面切换键
    g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);


    //setStyleSheet("QWidget { background-image: url(:/images/background); }");
    setPalette(QPalette(QColor(Qt::white)));
    // 设置窗口属性为无边框且始终保持最大化
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    showFullScreen();

    int icon_size = 48;

    // 创建一个按钮用于关闭窗口
    setup_icon_button(ui->close, "quit", icon_size, "black");
    connect(ui->close, &QPushButton::clicked, this, &QWidget::close);

    setup_icon_button(ui->nightButton, "dark", icon_size * 0.95, "black");
    connect(ui->nightButton, &QPushButton::clicked, [=](){
        is_night = !is_night;
        if(!is_night)
        {
            setPalette(QPalette(QColor(Qt::white)));
            ui->datelabel->setStyleSheet("font-size: 54px; color: #000000;");
            ui->timeLabel->setStyleSheet("font-size: 96px; color: #000000;");
            ui->famous->setStyleSheet("font-size: 36px; color: #000000;");
            ui->elapsedTimeLabel->setStyleSheet("font-size: 36px; color: #000000;");
            setup_icon_button(ui->close, "quit", icon_size, "black");
            setup_icon_button(ui->picture, "image", icon_size, "black");
            setup_icon_button(ui->renew, "refresh", icon_size * 1.15, "black");
            setup_icon_button(ui->nightButton, "dark", icon_size * 0.95, "black");
        }
        else {
            setPalette(QPalette(QColor(Qt::black)));
            ui->datelabel->setStyleSheet("font-size: 54px; color: #ffffff;");
            ui->timeLabel->setStyleSheet("font-size: 96px; color: #ffffff;");
            ui->famous->setStyleSheet("font-size: 36px; color: #ffffff;");
            ui->elapsedTimeLabel->setStyleSheet("font-size: 36px; color: #ffffff;");
            setup_icon_button(ui->close, "quit", icon_size, "white");
            setup_icon_button(ui->picture, "image", icon_size, "white");
            setup_icon_button(ui->renew, "refresh", icon_size * 1.15, "white");
            setup_icon_button(ui->nightButton, "light", icon_size * 0.95, "white");
        }
    });

    setup_icon_button(ui->picture, "image", icon_size, "black");
    connect(ui->picture, &QPushButton::clicked,[=]{
        picturetype=(picturetype+1)%4;
        repaint();
    });

    setup_icon_button(ui->renew, "refresh", icon_size * 1.15, "black");
    connect(ui->renew, &QPushButton::clicked,[=]{
        Network *n = new Network("https://v1.hitokoto.cn?c=d&c=i&c=k");
        n->post();
        connect(n, &Network::succeeded, [this] (QString reply) {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(reply.toUtf8());
            if (!jsonDoc.isNull()) {
                QJsonObject jsonObj = jsonDoc.object();
                QString author = jsonObj["from_who"].toString();
                QString text = "“" + jsonObj["hitokoto"].toString() + "”";
                ui->famous->setText(text);
                // if (author != "") {
                //     window->ui->famous->setText("——" + author);
                // }
            } else {
                qDebug() << "Failed to parse JSON.";
            }
        });
    });

    // 记录当前时间作为窗口打开时的时间
    startTime = QDateTime::currentDateTime();


    // 设置时间标签的属性
    ui->datelabel->setAlignment(Qt::AlignCenter);
    ui->datelabel->setStyleSheet("font-size: 54px; color: #000000;");
    // 设置时间标签的属性
    ui->timeLabel->setAlignment(Qt::AlignCenter);
    ui->timeLabel->setStyleSheet("font-size: 96px; color: #000000;");
    // 设置运行时间标签的属性
    ui->elapsedTimeLabel->setAlignment(Qt::AlignCenter);
    ui->elapsedTimeLabel->setStyleSheet("font-size: 54px; color: #000000;");



    // 定时器每秒更新一次时间
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &FocusWindow::updateTime);
    timer->start(1000); // 每秒触发一次


    // 更新一次时间标签，以确保启动时显示当前时间
    updateTime();



    ui->famous->setStyleSheet("font-size: 36px; color: #000000;");
    Network *n = new Network("https://v1.hitokoto.cn/");
    n->post();
    connect(n, &Network::succeeded, [this] (QString reply) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply.toUtf8());
        if (!jsonDoc.isNull()) {
            QJsonObject jsonObj = jsonDoc.object();
            QString author = jsonObj["from_who"].toString();
            QString text = "“" + jsonObj["hitokoto"].toString() + "”";
            ui->famous->setText(text);
            // if (author != "") {
            //     window->ui->famous->setText("——" + author);
            // }
        } else {
            qDebug() << "Failed to parse JSON.";
        }
    });

    //repaint();

    // // 创建一个中心部件并设置布局
    QHBoxLayout* layout1=new QHBoxLayout();
    QLabel* hh =new QLabel("请返回专注模式");
    hh->setStyleSheet("font-size: 146px; color: #000000;");
    hh->setAlignment(Qt::AlignCenter);
    layout1->addWidget(hh);
    QWidget *centralWidget = new QWidget(&a);
    centralWidget->setLayout(layout1);
    a.setCentralWidget(centralWidget);
}


void FocusWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    if(picturetype==1){
        QPainter painter(this);
        QPixmap pixmap(":/images/background"); // 加载图片
        painter.setOpacity(0.5);
        QRect rect(QPoint((width() - pixmap.width()) / 2, (height() - pixmap.height()) / 2), pixmap.size()); // 计算居中的位置
        painter.drawPixmap(rect, pixmap); // 在计算出的位置绘制图片
    }
    if(picturetype==2){
        QPainter painter(this);
        QPixmap pixmap(":/images/background2"); // 加载图片
        pixmap = pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.setOpacity(0.5);
        QRect rect(QPoint((width() - pixmap.width()) / 2, (height() - pixmap.height()) / 2), pixmap.size()); // 计算居中的位置
        painter.drawPixmap(rect, pixmap); // 在计算出的位置绘制图片
    }
    if(picturetype==3){
        QPainter painter(this);
        QPixmap pixmap(":/images/background3"); // 加载图片
        pixmap = pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.setOpacity(0.5);
        QRect rect(QPoint((width() - pixmap.width()) / 2, (height() - pixmap.height()) / 2), pixmap.size()); // 计算居中的位置
        painter.drawPixmap(rect, pixmap); // 在计算出的位置绘制图片
    }
}


FocusWindow::~FocusWindow()
{
    // 卸载钩子
    UnhookWindowsHookEx(g_hKeyboardHook);
    a.close();

    delete ui;
}

void FocusWindow::closeEvent(QCloseEvent *event)
{
    // 暂停定时器以允许用户与消息框交互
    checkTimer->start(2000);

    if(canclose==0){
        event->ignore();
        return ;
    }

    // 禁用窗口关闭事件，只有点击按钮时才能关闭
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, "退出",
                                                               "确定退出专注模式？",
                                                               QMessageBox::No | QMessageBox::Yes,
                                                               QMessageBox::No);

    if (resBtn != QMessageBox::Yes) {
        // 用户取消关闭窗口，重新启动定时器
        checkTimer->start(2000);

        event->ignore();
    } else {
        a.close();
        delete checkTimer;
        UnhookWindowsHookEx(g_hKeyboardHook);
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
    if(flag==0)
    {
        QString elapsedTimeString = QString("已专注时间: %1:%2:%3")
                                        .arg(hours, 2, 10, QChar('0'))
                                        .arg(minutes, 2, 10, QChar('0'))
                                        .arg(seconds, 2, 10, QChar('0'));
        ui->elapsedTimeLabel->setText(elapsedTimeString);
    }
    else
    {
        if(elapsedSeconds>=countdowntime)
        {
            ui->close->setEnabled(true);
            canclose=1;
        }
        elapsedSeconds=fmax(0,countdowntime-elapsedSeconds);
        int hours = elapsedSeconds / 3600;
        int minutes = (elapsedSeconds % 3600) / 60;
        int seconds = elapsedSeconds % 60;
        QString elapsedTimeString = QString("还需专注时间: %1:%2:%3")
                                        .arg(hours, 2, 10, QChar('0'))
                                        .arg(minutes, 2, 10, QChar('0'))
                                        .arg(seconds, 2, 10, QChar('0'));
        ui->elapsedTimeLabel->setText(elapsedTimeString);
    }
}
