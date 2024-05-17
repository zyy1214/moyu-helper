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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setPalette(QPalette(QColor(Qt::white)));
    data = new Data();
    rw = new RecordWindow(data, this);
    mw = new ModWindow(data, this);
    ui->stackedWidget->addWidget(rw);
    ui->stackedWidget->addWidget(mw);
    ui->stackedWidget->setCurrentWidget(rw);
    //rw->show();
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

    QStackedWidget *pagesWidget;
    QPushButton *accountButton;
    QPushButton *interfaceButton;
    QMainWindow *window;
};

void SettingsDialog::setupUi()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    accountButton = new QPushButton(tr("账号设置"));
    interfaceButton = new QPushButton(tr("界面设置"));
    buttonsLayout->addWidget(accountButton);
    buttonsLayout->addWidget(interfaceButton);

    QWidget *scrollWidget = new QWidget;
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);

    pagesWidget = new QStackedWidget;

    QWidget *accountPage = new QWidget;
    QLabel *accountLabel = new QLabel(get_value("username"));
    QVBoxLayout *accountPageLayout = new QVBoxLayout(accountPage);
    accountPageLayout->addWidget(accountLabel);

    QPushButton *logoutButton = new QPushButton(tr("退出登录"));
    logoutButton->setStyleSheet("color: red;");
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
    QLabel *interfaceLabel = new QLabel(tr("Interface Settings Page"));
    QVBoxLayout *interfacePageLayout = new QVBoxLayout(interfacePage);
    interfacePageLayout->addWidget(interfaceLabel);
    pagesWidget->addWidget(interfacePage);

    scrollLayout->addWidget(pagesWidget);

    connect(accountButton, &QPushButton::clicked, this, &SettingsDialog::changePage);
    connect(interfaceButton, &QPushButton::clicked, this, &SettingsDialog::changePage);

    mainLayout->addLayout(buttonsLayout);
    mainLayout->addWidget(scrollWidget);

    setWindowTitle(tr("Settings"));
}

void SettingsDialog::changePage()
{
    if (sender() == accountButton)
        pagesWidget->setCurrentIndex(0);
    else if (sender() == interfaceButton)
        pagesWidget->setCurrentIndex(1);
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
    QAction *action_feedback = menu->addAction("反馈");
    QAction *action_about = menu->addAction("关于");
    connect(action_settings, &QAction::triggered, this, [this] () {
        SettingsDialog *dialog = new SettingsDialog(nullptr, this);
        dialog->show();
    });
    connect(action_feedback, &QAction::triggered, this, [this] () {

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
    ui->button_statistics->setStyleSheet("border: none; outline: none; padding: 5;");
    ui->button_theme->setStyleSheet("border: none; outline: none; padding: 5;");
}

void MainWindow::on_button_record_clicked()
{
    clear_page_choice();
    ui->button_record->setStyleSheet("border: none; outline: none; padding: 5; background-color: rgb(192, 220, 243);");
    ui->stackedWidget->setCurrentWidget(rw);
}

void MainWindow::on_button_mod_clicked()
{
    clear_page_choice();
    ui->button_mod->setStyleSheet("border: none; outline: none; padding: 5; background-color: rgb(192, 220, 243);");
    ui->stackedWidget->setCurrentWidget(mw);
}




