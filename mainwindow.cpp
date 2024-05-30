#include "focuswindow.h"
#include "loginwindow.h"
#include "mainwindow.h"
#include "recordwindow.h"
#include "ui_mainwindow.h"

#include "data_storage.h"
#include "uitools.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QListWidgetItem>
#include <QMenu>
#include <QPalette>
#include <QScrollArea>
#include <QScrollBar>
#include <QWheelEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setPalette(QPalette(QColor(Qt::white)));
    data = new Data();
    load_data(data);
    rw = new RecordWindow(data, this);
    mw = new ModWindow(data, this);
    cw = new ChartWindow(data, this);
    tw = new ThemeWindow(this);
    ui->stackedWidget->addWidget(rw);
    ui->stackedWidget->addWidget(mw);
    ui->stackedWidget->addWidget(cw);
    ui->stackedWidget->addWidget(tw);
    ui->stackedWidget->setCurrentWidget(rw);
    setWindowTitle("摸鱼小助手 - 主页");
}

MainWindow::~MainWindow()
{
    delete ui;
}

class SettingsDialog : public QDialog
{

public:
    SettingsDialog(QWidget *parent, QMainWindow *window)
        : QDialog(parent), window(window)
    {
        setupUi();
    }

private slots:
    void changePage();

private:
    void setupUi();

    void wheelEvent(QWheelEvent *event);

    QStackedWidget *pagesWidget;
    QPushButton *accountButton;
    QPushButton *interfaceButton;
    QMainWindow *window;

    void cancel_button_selection() {
        accountButton->setStyleSheet("QPushButton {background-color: transparent; border: none; font-size: 18px; color: black; }");
        interfaceButton->setStyleSheet("QPushButton {background-color: transparent; border: none; font-size: 18px; color: black; }");
    }
    void set_button_selection(int index) {
        cancel_button_selection();
        QPushButton *button = index == 0 ? accountButton : interfaceButton;
        button->setStyleSheet("QPushButton {background-color: transparent; border: none; font-size: 18px; color: #1770E4; }");
    }
    void set_page_selection(int index) {
        pagesWidget->setCurrentIndex(index);
        set_button_selection(index);
    }
};

void SettingsDialog::setupUi()
{
    setPalette(QPalette(QColor(Qt::white)));
    setFixedSize(500, 300);
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    accountButton = new QPushButton("账号设置");
    interfaceButton = new QPushButton("界面设置");
    set_button_selection(0);
    buttonsLayout->addSpacing(20);
    buttonsLayout->addWidget(accountButton);
    buttonsLayout->addSpacing(10);
    buttonsLayout->addWidget(interfaceButton);
    buttonsLayout->setAlignment(Qt::AlignTop);

    QWidget *scrollWidget = new QWidget;
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);

    pagesWidget = new QStackedWidget;

    QWidget *accountPage = new QWidget;
    QLabel *accountLabel = new QLabel(get_value("username"));
    accountLabel->setStyleSheet("font-size: 24px");
    QVBoxLayout *accountPageLayout = new QVBoxLayout(accountPage);
    accountPageLayout->setAlignment(Qt::AlignTop);
    accountPageLayout->addWidget(accountLabel);
    accountPageLayout->addSpacing(15);

    QPushButton *logoutButton = new QPushButton("退出登录");
    logoutButton->setStyleSheet("color: red; font-size: 14px;"
                                "padding-left: 15px; padding-right: 15px; padding-top: 6px; padding-bottom: 6px;");
    logoutButton->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
    accountPageLayout->addWidget(logoutButton);

    connect(logoutButton, &QPushButton::clicked, [=] () {
        show_confirm(window, "退出登录", "退出登录后需联网重新登录，方能继续使用软件。", [=] (QMainWindow *window) {
            this->close();
            save_value("username", "");
            save_value("password", "");
            save_value("token", "");
            LoginWindow *lw = new LoginWindow();
            lw->show();
            window->close();
        });
    });

    pagesWidget->addWidget(accountPage);

    QWidget *interfacePage = new QWidget;
    QLabel *interfaceLabel = new QLabel("暂无任何可设置项。");
    QVBoxLayout *interfacePageLayout = new QVBoxLayout(interfacePage);
    interfacePageLayout->setAlignment(Qt::AlignTop);
    interfacePageLayout->addWidget(interfaceLabel);
    pagesWidget->addWidget(interfacePage);

    scrollLayout->addWidget(pagesWidget);

    connect(accountButton, &QPushButton::clicked, this, &SettingsDialog::changePage);
    connect(interfaceButton, &QPushButton::clicked, this, &SettingsDialog::changePage);

    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(scrollWidget);

    setWindowTitle("设置");
}

void SettingsDialog::changePage()
{
    if (sender() == accountButton) {
        set_page_selection(0);
    }
    else if (sender() == interfaceButton) {
        set_page_selection(1);
    }
}

void SettingsDialog::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0) {
        int new_index = pagesWidget->currentIndex() - 1;
        if (new_index < 0) return;
        set_page_selection(new_index);
    } else {
        int new_index = pagesWidget->currentIndex() + 1;
        if (new_index >= pagesWidget->count()) return;
        set_page_selection(new_index);
    }
    event->accept();
}

void MainWindow::on_button_menu_clicked()
{
    QMenu *menu = new QMenu(this);

    QFont font;
    font.setPointSize(16);
    menu->setFont(font);

    // QPalette palette = menu->palette();
    // palette.setColor(QPalette::Window, Qt::white);
    // menu->setPalette(palette);
    menu->setStyleSheet("QMenu { background-color: 0xdddddd; border: none; padding: 0px; }"
                        "QMenu::item:selected { background-color: rgb(192, 220, 243); }"
                        "QMenu::item { padding-top: 8px; padding-bottom: 8px; padding-left: 25px; padding-right: 25px}");

    QAction *action_settings = menu->addAction("设置");
    QAction *focus_mode = menu->addAction("专注模式");
    QAction *action_feedback = menu->addAction("反馈与帮助");
    QAction *action_about = menu->addAction("关于");
    connect(action_settings, &QAction::triggered, this, [this] () {
        SettingsDialog *dialog = new SettingsDialog(nullptr, this);
        dialog->show();
    });
    connect(focus_mode, &QAction::triggered, this, [this] () {
        //show_info("专注模式", "该功能正在开发中，将在未来版本中更新，敬请期待……");
        FocusWindow *window = new FocusWindow();
        window->show();
    });
    connect(action_feedback, &QAction::triggered, this, [this] () {
        show_info("反馈与帮助", "您可以联系 zyy@stu.pku.edu.cn 以反馈问题或获取帮助。");
    });
    connect(action_about, &QAction::triggered, this, [this]() {
        show_info("关于", "软件名称：摸鱼小助手\n软件版本：v0.0.1\n制作人员：张远洋 王之略 刘曜玮");
    });

    QPoint cursorPos = QCursor::pos();
    QPoint pos = cursorPos - QPoint(0, menu->sizeHint().height());
    //QPoint pos = ui->button_menu->mapToGlobal(QPoint(0, 0));
    menu->exec(pos);
}

void MainWindow::clear_page_choice() {
    ui->button_record->setStyleSheet("border: none; outline: none; padding: 5;");
    ui->button_mod->setStyleSheet("border: none; outline: none; padding: 5;");
    ui->button_chart->setStyleSheet("border: none; outline: none; padding: 5;");
    ui->button_theme->setStyleSheet("border: none; outline: none; padding: 5;");
}

void MainWindow::on_button_record_clicked()
{
    clear_page_choice();
    ui->button_record->setStyleSheet("border: none; outline: none; padding: 5; background-color: rgb(192, 220, 243);");
    ui->stackedWidget->setCurrentWidget(rw);
    setWindowTitle("摸鱼小助手 - 主页");
}

void MainWindow::on_button_mod_clicked()
{
    clear_page_choice();
    ui->button_mod->setStyleSheet("border: none; outline: none; padding: 5; background-color: rgb(192, 220, 243);");
    ui->stackedWidget->setCurrentWidget(mw);
    setWindowTitle("摸鱼小助手 - 模板");
}

void MainWindow::on_button_chart_clicked()
{
    clear_page_choice();
    ui->button_chart->setStyleSheet("border: none; outline: none; padding: 5; background-color: rgb(192, 220, 243);");
    ui->stackedWidget->setCurrentWidget(cw);
    setWindowTitle("摸鱼小助手 - 统计");
}


void MainWindow::on_button_theme_clicked()
{
    clear_page_choice();
    ui->button_theme->setStyleSheet("border: none; outline: none; padding: 5; background-color: rgb(192, 220, 243);");
    ui->stackedWidget->setCurrentWidget(tw);
    setWindowTitle("摸鱼小助手 - 主题");
}

