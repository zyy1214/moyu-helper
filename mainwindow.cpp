#include "focuswindow.h"
#include "loginwindow.h"
#include "mainwindow.h"
#include "recordwindow.h"
#include "ui_mainwindow.h"

#include "data_storage.h"
#include "network.h"
#include "uitools.h"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QListWidgetItem>
#include <QMenu>
#include <QPalette>
#include <QScrollArea>
#include <QScrollBar>
#include <QWheelEvent>
#include <QButtonGroup>
#include <QRadioButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setPalette(QPalette(QColor(Qt::white)));
    data = new Data();
    load_data(data);
    sync_settings(data);
    rw = new RecordWindow(data, this);
    mw = new ModWindow(data, this);
    cw = new ChartWindow(data, this);
    tw = new ThemeWindow(this,this);
    ui->stackedWidget->addWidget(rw);
    ui->stackedWidget->addWidget(mw);
    ui->stackedWidget->addWidget(cw);
    ui->stackedWidget->addWidget(tw);
    ui->stackedWidget->setCurrentWidget(rw);
    setWindowTitle("摸鱼小助手 - 主页");
    setWindowIcon(QIcon(":/images/icon"));

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/images/icon"));
    QMenu *menu = new QMenu(this);
    QAction *restoreAction = new QAction("恢复窗口", this);
    QAction *quitAction = new QAction("退出", this);

    connect(restoreAction, &QAction::triggered, this, &MainWindow::showNormal);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    menu->addAction(restoreAction);
    menu->addAction(quitAction);

    trayIcon->setContextMenu(menu);
    trayIcon->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

class ModifyPasswordDialog : public QDialog {
public:
    ModifyPasswordDialog(QWidget *parent, QMainWindow *window)
        : QDialog(parent), window(window)
    {
        setWindowTitle("修改密码");
        setPalette(QPalette(QColor(Qt::white)));
        setFixedWidth(320);
        QVBoxLayout *modify_password_layout = new QVBoxLayout(this);
        setLayout(modify_password_layout);
        modify_password_layout->setAlignment(Qt::AlignCenter);
        // modify_password_layout->setContentsMargins(30, 0, 30, 0);

        modify_password_layout->addSpacing(30);

        QHBoxLayout *titleLayout = new QHBoxLayout();
        QLabel *titleLabel = new QLabel("修改密码", this);
        titleLabel->setWordWrap(true);
        titleLabel->setStyleSheet("font-weight: bold; font-size: 24px;");
        titleLayout->addSpacing(25);
        titleLayout->addWidget(titleLabel);
        titleLayout->addSpacing(25);
        modify_password_layout->addItem(titleLayout);
        modify_password_layout->addSpacing(25);

        QHBoxLayout *inputLayoutH = new QHBoxLayout(this);

        QVBoxLayout *inputLayoutV = new QVBoxLayout(this);
        original_password_edit = new QLineEdit(this);
        original_password_edit->setPlaceholderText("请输入原密码");
        original_password_edit->setEchoMode(QLineEdit::Password);
        QFont editFont = original_password_edit->font();
        editFont.setPointSize(14);
        original_password_edit->setFont(editFont);
        inputLayoutV->addWidget(original_password_edit);

        new_password_edit = new QLineEdit(this);
        new_password_edit->setPlaceholderText("请输入新密码");
        new_password_edit->setEchoMode(QLineEdit::Password);
        new_password_edit->setFont(editFont);
        inputLayoutV->addWidget(new_password_edit);

        confirm_password_edit = new QLineEdit(this);
        confirm_password_edit->setPlaceholderText("请再次输入新密码");
        confirm_password_edit->setEchoMode(QLineEdit::Password);
        confirm_password_edit->setFont(editFont);
        inputLayoutV->addWidget(confirm_password_edit);

        inputLayoutV->setSpacing(8);

        inputLayoutH->addSpacing(25);
        inputLayoutH->addItem(inputLayoutV);
        inputLayoutH->addSpacing(25);

        modify_password_layout->addItem(inputLayoutH);

        modify_password_layout->addSpacing(30);

        confirm_layout = new ConfirmLayout(this);
        modify_password_layout->addItem(confirm_layout);
        modify_password_layout->addSpacing(35);

        connect(confirm_layout, &ConfirmLayout::canceled, this, &QDialog::close);
        connect(confirm_layout, &ConfirmLayout::confirmed, this, [=] () {
            if (new_password_edit->text().length() < 6) {
                show_warning("修改密码失败", "密码长度至少为6位。");
                return;
            }
            if (new_password_edit->text() != confirm_password_edit->text()) {
                show_warning("修改密码失败", "请确保两次输入的密码内容一致。");
                return;
            }
            confirm_layout->setEnabled(false);
            Network *n = new Network("https://geomedraw.com/qt/modify_password");
            n->add_data("username", get_value("username"));
            n->add_data("original_password", original_password_edit->text());
            n->add_data("new_password", new_password_edit->text());
            n->post();
            connect(n, &Network::succeeded, [=] (QString reply) {
                if (reply != "true") {
                    show_warning("修改密码失败", reply);
                    confirm_layout->setEnabled(true);
                } else {
                    close();
                    show_info("修改密码成功", "修改密码成功。");
                }
            });
            connect(n, &Network::failed, [=] () {
                show_warning("网络错误", "网络连接失败。");
            });
        });

        QString textEditStyle = "QLineEdit { border: 1px solid gray; border-radius: 5px; "
                                "padding-left: 10px; padding-right: 10px; padding-top: 5px; padding-bottom: 5px; }"
                                "QLineEdit:focus { border-color: #add8e6; }";
        original_password_edit->setStyleSheet(textEditStyle);
        new_password_edit->setStyleSheet(textEditStyle);
        confirm_password_edit->setStyleSheet(textEditStyle);
    }

private:
    QMainWindow *window;
    QLineEdit *original_password_edit;
    QLineEdit *new_password_edit;
    QLineEdit *confirm_password_edit;
    ConfirmLayout *confirm_layout;
};

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
    QPushButton *overallButton;
    QMainWindow *window;

    void cancel_button_selection() {
        accountButton->setStyleSheet("QPushButton {background-color: transparent; border: none; font-size: 18px; color: black; }");
        overallButton->setStyleSheet("QPushButton {background-color: transparent; border: none; font-size: 18px; color: black; }");
    }
    void set_button_selection(int index) {
        cancel_button_selection();
        QPushButton *button = index == 0 ? accountButton : overallButton;
        button->setStyleSheet("QPushButton {background-color: transparent; border: none; font-size: 18px; color: #1770E4; }");
    }
    void set_page_selection(int index) {
        pagesWidget->setCurrentIndex(index);
        set_button_selection(index);
    }
};

class FocusDialog : public QDialog {

public:
    FocusDialog(QWidget *parent = nullptr)
        : QDialog(parent) {

        setPalette(QPalette(QColor(Qt::white)));
        QFont font("Microsoft YaHei UI", 12);//字体

        setWindowTitle(QString("添加模板"));
        setFixedSize(350, 200);
        flag=0;
        QRadioButton *normalButton = new QRadioButton("普通模式");
        normalButton->setFont(font);
        QRadioButton *countdownButton = new QRadioButton("倒计时模式");
        countdownButton->setFont(font);
        if(flag==0)
            normalButton->setChecked(true);
        else
            countdownButton->setChecked(true);

        // 将Radio button添加到按钮组
        buttonGroup.addButton(countdownButton);
        buttonGroup.addButton(normalButton);

        QHBoxLayout *buttonlayout=new QHBoxLayout;
        buttonlayout->addWidget(normalButton);
        buttonlayout->addWidget(countdownButton);

        QHBoxLayout *reallayout = new QHBoxLayout;
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addItem(new QSpacerItem(30, 30, QSizePolicy::Fixed, QSizePolicy::Fixed));
        layout->addLayout(buttonlayout);
        QIntValidator *validator = new QIntValidator(0, INT_MAX); // 0 到 INT_MAX 的整数

        // 创建文本输入框
        QLabel *guide=new QLabel("倒计时：");
        guide->setFont(font);
        QLabel *label_hour=new QLabel("小时");
        label_hour->setFont(font);
        QLabel *label_min=new QLabel("分钟");
        label_min->setFont(font);
        QLabel *label_second=new QLabel("秒");
        label_second->setFont(font);
        QLineEdit *lineEdit_hour = new QLineEdit();
        lineEdit_hour->setFont(font);
        lineEdit_hour->setValidator(validator);
        QLineEdit *lineEdit_min = new QLineEdit();
        lineEdit_min->setFont(font);
        lineEdit_min->setValidator(validator);
        QLineEdit *lineEdit_second=new QLineEdit();
        lineEdit_second->setFont(font);
        lineEdit_second->setValidator(validator);
        QHBoxLayout *input=new QHBoxLayout;
        input->addWidget(guide);
        input->addWidget(lineEdit_hour);
        input->addWidget(label_hour);
        input->addWidget(lineEdit_min);
        input->addWidget(label_min);
        input->addWidget(lineEdit_second);
        input->addWidget(label_second);

        // 添加确定和取消按钮
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *okButton = new QPushButton("确定", this);
        okButton->setStyleSheet("color: black; font-size: 14px;"
                                "padding-left: 25px; padding-right: 25px; padding-top: 6px; padding-bottom: 6px;");
        okButton->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
        QPushButton *cancelButton = new QPushButton("取消", this);
        cancelButton->setStyleSheet("color: black; font-size: 14px;"
                                    "padding-left: 25px; padding-right: 25px; padding-top: 6px; padding-bottom: 6px;");
        cancelButton->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
        buttonLayout->setAlignment(Qt::AlignRight);
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addSpacing(8);
        buttonLayout->addWidget(okButton);
        buttonLayout->addSpacing(8);
        buttonLayout->setContentsMargins(0, 5, 0, 5);
        // 连接确定按钮的点击事件
        connect(okButton, &QPushButton::clicked, [=]() {
            if(flag==1)
            {
                ConfirmDialog *confirm = new ConfirmDialog(nullptr, "开启专注模式", "确认开启专注模式吗？开启后，在倒计时结束前，您将无法退出专注模式。");
                connect(confirm, &ConfirmDialog::confirmed, [=] () {
                    close();
                    int min = lineEdit_min->text().toInt();
                    int hour = lineEdit_hour->text().toInt();
                    int second = lineEdit_second->text().toInt();
                    int hh=3600*hour+60*min+second;
                    FocusWindow *focus = new FocusWindow(1,hh);
                    focus->show();
                });
                confirm->exec();
            }
            else
            {
                FocusWindow *focus = new FocusWindow(0);
                focus->show();
                close();
            }
        });
        connect(cancelButton, &QPushButton::clicked,[=](){
            close();
        });
        QSpacerItem *spacer=new QSpacerItem(30, 100, QSizePolicy::Fixed, QSizePolicy::Expanding);
        layout->addItem(spacer);
        layout->addLayout(input);
        layout->addSpacing(10);
        layout->addLayout(buttonLayout);


        guide->hide();
        lineEdit_hour->hide();
        lineEdit_min->hide();
        lineEdit_second->hide();
        label_second->hide();
        label_min->hide();
        label_hour->hide();


        connect(normalButton,&QRadioButton::toggled,[=](){
            flag=0;
            guide->hide();
            lineEdit_hour->hide();
            lineEdit_min->hide();
            lineEdit_second->hide();
            label_second->hide();
            label_min->hide();
            label_hour->hide();

        });
        connect(countdownButton,&QRadioButton::toggled,[=](){
            flag=1;
            guide->show();
            lineEdit_hour->show();
            lineEdit_min->show();
            lineEdit_second->show();
            label_second->show();
            label_min->show();
            label_hour->show();
        });

        reallayout->addItem(new QSpacerItem(30, 30, QSizePolicy::Fixed, QSizePolicy::Minimum));
        reallayout->addLayout(layout);
        reallayout->addItem(new QSpacerItem(30, 30, QSizePolicy::Fixed, QSizePolicy::Minimum));

        setLayout(reallayout);
    }
    // void setupui(){
    //     QFont font("Microsoft YaHei UI", 12);//字体

    //     // 创建Radio button组
    //     QRadioButton *normalButton = new QRadioButton("普通模式");
    //     normalButton->setFont(font);
    //     QRadioButton *countdownButton = new QRadioButton("倒计时模式");
    //     countdownButton->setFont(font);
    //     if(flag==0)
    //         normalButton->setChecked(true);
    //     else
    //         countdownButton->setChecked(true);
    //     connect(normalButton,&QRadioButton::toggled,[=](){
    //         flag=0;
    //         setupui();
    //     });
    //     connect(countdownButton,&QRadioButton::toggled,[=](){
    //         flag=1;
    //         setupui();
    //     });

    //     // 将Radio button添加到按钮组
    //     buttonGroup.addButton(countdownButton);
    //     buttonGroup.addButton(normalButton);

    //     QHBoxLayout *buttonlayout=new QHBoxLayout;
    //     buttonlayout->addWidget(normalButton);
    //     buttonlayout->addWidget(countdownButton);

    //     QHBoxLayout *reallayout = new QHBoxLayout;
    //     QVBoxLayout *layout = new QVBoxLayout;
    //     layout->addLayout(buttonlayout);
    //     if(flag==1)
    //     {

    //         QIntValidator *validator = new QIntValidator(0, INT_MAX); // 0 到 INT_MAX 的整数

    //         // 创建文本输入框
    //             QLabel *guide=new QLabel("倒计时：");
    //         QLabel *label_hour=new QLabel("小时");
    //         label_hour->setFont(font);
    //         QLabel *label_min=new QLabel("分钟");
    //         label_min->setFont(font);
    //         QLabel *label_second=new QLabel("秒");
    //         label_second->setFont(font);
    //         QLineEdit *lineEdit_hour = new QLineEdit();
    //         lineEdit_hour->setFont(font);
    //         lineEdit_hour->setValidator(validator);
    //         QLineEdit *lineEdit_min = new QLineEdit();
    //         lineEdit_min->setFont(font);
    //         lineEdit_min->setValidator(validator);
    //         QLineEdit *lineEdit_second=new QLineEdit();
    //         lineEdit_second->setFont(font);
    //         lineEdit_second->setValidator(validator);
    //         QHBoxLayout *input=new QHBoxLayout;
    //         input->addWidget(guide);
    //         input->addWidget(lineEdit_hour);
    //         input->addWidget(label_hour);
    //         input->addWidget(lineEdit_min);
    //         input->addWidget(label_min);
    //         input->addWidget(lineEdit_second);
    //         input->addWidget(label_second);

    //         // 添加确定和取消按钮
    //         QHBoxLayout *buttonLayout = new QHBoxLayout();
    //         QPushButton *okButton = new QPushButton("确定", this);
    //         okButton->setStyleSheet("color: black; font-size: 14px;"
    //                                 "padding-left: 25px; padding-right: 25px; padding-top: 6px; padding-bottom: 6px;");
    //         okButton->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
    //         QPushButton *cancelButton = new QPushButton("取消", this);
    //         cancelButton->setStyleSheet("color: black; font-size: 14px;"
    //                                     "padding-left: 25px; padding-right: 25px; padding-top: 6px; padding-bottom: 6px;");
    //         cancelButton->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
    //         buttonLayout->setAlignment(Qt::AlignRight);
    //         buttonLayout->addWidget(cancelButton);
    //         buttonLayout->addSpacing(8);
    //         buttonLayout->addWidget(okButton);
    //         buttonLayout->addSpacing(8);
    //         buttonLayout->setContentsMargins(0, 5, 0, 5);
    //         // 连接确定按钮的点击事件
    //         connect(okButton, &QPushButton::clicked, [=]() {
    //             int min = lineEdit_min->text().toInt();
    //             int hour = lineEdit_hour->text().toInt();
    //             int second = lineEdit_second->text().toInt();
    //             int hh=3600*hour+60*min+second;
    //             FocusWindow *focus = new FocusWindow(1,hh);
    //             focus->show();
    //             close();
    //         });
    //         connect(cancelButton, &QPushButton::clicked,[=](){
    //             close();
    //         });
    //         layout->addLayout(input);
    //         layout->addSpacing(10);
    //         layout->addLayout(buttonLayout);
    //     }
    //     else
    //     {
    //         // 添加确定和取消按钮
    //         QHBoxLayout *buttonLayout = new QHBoxLayout();
    //         QPushButton *okButton = new QPushButton("确定", this);
    //         okButton->setStyleSheet("color: black; font-size: 14px;"
    //                                 "padding-left: 25px; padding-right: 25px; padding-top: 6px; padding-bottom: 6px;");
    //         okButton->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
    //         QPushButton *cancelButton = new QPushButton("取消", this);
    //         cancelButton->setStyleSheet("color: black; font-size: 14px;"
    //                                     "padding-left: 25px; padding-right: 25px; padding-top: 6px; padding-bottom: 6px;");
    //         cancelButton->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
    //         buttonLayout->setAlignment(Qt::AlignRight);
    //         buttonLayout->addWidget(cancelButton);
    //         buttonLayout->addSpacing(8);
    //         buttonLayout->addWidget(okButton);
    //         buttonLayout->addSpacing(8);
    //         buttonLayout->setContentsMargins(0, 5, 0, 5);
    //         // 连接确定按钮的点击事件
    //         connect(okButton, &QPushButton::clicked, [=]() {
    //             FocusWindow *focus = new FocusWindow(0);
    //             focus->show();
    //             close();
    //         });
    //         connect(cancelButton, &QPushButton::clicked,[=](){
    //             close();
    //         });
    //         layout->addSpacing(10);
    //         layout->addLayout(buttonLayout);
    //     }





    //     reallayout->addItem(new QSpacerItem(30, 30, QSizePolicy::Fixed, QSizePolicy::Minimum));
    //     reallayout->addLayout(layout);
    //     reallayout->addItem(new QSpacerItem(30, 30, QSizePolicy::Fixed, QSizePolicy::Minimum));

    //     setLayout(reallayout);

    // }

private:
    QButtonGroup buttonGroup;
    bool flag;
};

void SettingsDialog::setupUi()
{
    setPalette(QPalette(QColor(Qt::white)));
    setFixedSize(500, 300);
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    accountButton = new QPushButton("账号设置");
    overallButton = new QPushButton("全局设置");
    set_button_selection(0);
    buttonsLayout->addSpacing(20);
    buttonsLayout->addWidget(accountButton);
    buttonsLayout->addSpacing(10);
    buttonsLayout->addWidget(overallButton);
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

    QPushButton *modify_password_button = create_standard_button("修改密码", 14);
    accountPageLayout->addWidget(modify_password_button);
    accountPageLayout->addSpacing(5);
    connect(modify_password_button, &QPushButton::clicked, [=] () {
        ModifyPasswordDialog *dialog = new ModifyPasswordDialog(nullptr, window);
        dialog->show();
    });

    QPushButton *logoutButton = new QPushButton("退出登录");
    logoutButton->setStyleSheet("color: red; font-size: 14px;"
                                "padding-left: 15px; padding-right: 15px; padding-top: 6px; padding-bottom: 6px;");
    logoutButton->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
    accountPageLayout->addWidget(logoutButton);

    connect(logoutButton, &QPushButton::clicked, [=] () {
        ConfirmDialog *confirmDialog = new ConfirmDialog(window, "退出登录", "退出登录后需联网重新登录，方能继续使用软件。");
        connect(confirmDialog, &ConfirmDialog::confirmed, [=] () {
            this->close();
            save_value("username", "");
            save_value("password", "");
            save_value("token", "");
            LoginWindow *lw = new LoginWindow();
            lw->show();
            window->close();
        });
        confirmDialog->show();
    });

    pagesWidget->addWidget(accountPage);

    QWidget *overallPage = new QWidget;
    QVBoxLayout *overallPageLayout = new QVBoxLayout(overallPage);
    overallPageLayout->setAlignment(Qt::AlignTop);

    QHBoxLayout *data_unit_layout = new QHBoxLayout(overallPage);
    QLabel *data_unit_label = new QLabel("数据单位：", overallPage);
    data_unit_label->setStyleSheet("font-size: 14px");
    QComboBox *data_unit_combo = new QComboBox(overallPage);
    data_unit_combo->addItems({"1", "0.1", "0.01"});
    data_unit_combo->setCurrentText(get_value("data_unit", true, "1"));
    QFont font = data_unit_combo->font();
    font.setPixelSize(14);
    data_unit_combo->setFont(font);
    data_unit_layout->addWidget(data_unit_label);
    data_unit_layout->addSpacing(5);
    data_unit_layout->addWidget(data_unit_combo);
    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    data_unit_layout->addSpacerItem(spacer);
    overallPageLayout->addItem(data_unit_layout);
    overallPageLayout->addSpacing(10);
    connect(data_unit_combo, &QComboBox::currentIndexChanged, [=] (int index) {
        save_value("data_unit", data_unit_combo->currentText(), true);
        save_value("data_unit_time", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"), true);
        sync_settings(((MainWindow *) window)->data);
        emit ((MainWindow *) window)->data->settings_changed();
    });

    pagesWidget->addWidget(overallPage);

    scrollLayout->addWidget(pagesWidget);

    connect(accountButton, &QPushButton::clicked, this, &SettingsDialog::changePage);
    connect(overallButton, &QPushButton::clicked, this, &SettingsDialog::changePage);

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
    else if (sender() == overallButton) {
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
        FocusDialog aa;
        aa.exec();
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


void MainWindow::closeEvent(QCloseEvent *event) {
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }
}
