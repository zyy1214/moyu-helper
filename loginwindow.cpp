#include "loginwindow.h"
#include "mainwindow.h"
#include "recordwindow.h"
#include "data_storage.h"
#include "network.h"
#include "uitools.h"

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setStyleSheet("background-color: white;"); // 设置背景颜色为白色

    setWindowTitle("摸鱼小助手 - 登录");
    setWindowIcon(QIcon(":/images/icon"));

    // 禁止用户调整窗口大小
    setFixedSize(350, 450);

    // 创建堆叠窗口
    stackedWidget = new QStackedWidget(this);

    // 创建登录页面
    QWidget *loginPage = new QWidget(this);
    QVBoxLayout *loginLayout = new QVBoxLayout(loginPage);

    loginLayout->addSpacing(30);

    QLabel *loginLabel = new QLabel("登  录", this);
    loginLabel->setAlignment(Qt::AlignCenter);
    QFont loginFont = loginLabel->font();
    loginFont.setPointSize(24);
    loginLabel->setFont(loginFont);
    loginLayout->addWidget(loginLabel);

    loginLayout->addSpacing(25);

    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("请输入用户名");
    QFont editFont = usernameEdit->font();
    editFont.setPointSize(14);
    usernameEdit->setFont(editFont);
    loginLayout->addWidget(usernameEdit);

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFont(editFont);
    loginLayout->addWidget(passwordEdit);

    QSpacerItem *spacerItem1 = new QSpacerItem(30, 30, QSizePolicy::Minimum, QSizePolicy::Minimum);
    loginLayout->addItem(spacerItem1);

    loginButton = new QPushButton("立即登录", this);
    QFont buttonFont = loginButton->font();
    buttonFont.setPointSize(14);
    loginButton->setFont(buttonFont);
    //loginButton->setFixedSize(loginButton->sizeHint());
    QHBoxLayout *buttonLayout1 = new QHBoxLayout();
    buttonLayout1->addWidget(loginButton, 0, Qt::AlignHCenter);
    loginLayout->addLayout(buttonLayout1);

    QLabel *registerLabel = new QLabel("没有账号？<a href=\"#\">立即注册</a>", this);
    registerLabel->setAlignment(Qt::AlignCenter);
    QFont registerFont = registerLabel->font();
    registerFont.setPointSize(10);
    registerLabel->setFont(registerFont);
    loginLayout->addWidget(registerLabel);

    connect(registerLabel, &QLabel::linkActivated, [=]() {
        stackedWidget->setCurrentIndex(1);
        setWindowTitle("摸鱼小助手 - 注册");
    });

    stackedWidget->addWidget(loginPage);

    // 创建注册页面
    QWidget *registerPage = new QWidget(this);
    QVBoxLayout *registerLayout = new QVBoxLayout(registerPage);

    registerLayout->addSpacing(30);

    QLabel *registerTitleLabel = new QLabel("注  册", this);
    registerTitleLabel->setAlignment(Qt::AlignCenter);
    registerTitleLabel->setFont(loginFont);
    registerLayout->addWidget(registerTitleLabel);

    registerLayout->addSpacing(25);

    registerUsernameEdit = new QLineEdit(this);
    registerUsernameEdit->setPlaceholderText("请输入用户名");
    registerUsernameEdit->setFont(editFont);
    registerLayout->addWidget(registerUsernameEdit);

    registerPasswordEdit = new QLineEdit(this);
    registerPasswordEdit->setPlaceholderText("请输入密码");
    registerPasswordEdit->setEchoMode(QLineEdit::Password);
    registerPasswordEdit->setFont(editFont);
    registerLayout->addWidget(registerPasswordEdit);

    confirmPasswordEdit = new QLineEdit(this);
    confirmPasswordEdit->setPlaceholderText("请再次输入密码");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setFont(editFont);
    registerLayout->addWidget(confirmPasswordEdit);

    QSpacerItem *spacerItem2 = new QSpacerItem(30, 30, QSizePolicy::Minimum, QSizePolicy::Minimum);
    registerLayout->addItem(spacerItem2);

    registerButton = new QPushButton("立即注册", this);
    registerButton->setFont(buttonFont);
    //registerButton->setFixedSize(registerButton->sizeHint());
    QHBoxLayout *buttonLayout2 = new QHBoxLayout();
    buttonLayout2->addWidget(registerButton, 0, Qt::AlignHCenter);
    registerLayout->addLayout(buttonLayout2);

    QLabel *loginLinkLabel = new QLabel("已有账号？<a href=\"#\">立即登录</a>", this);
    loginLinkLabel->setAlignment(Qt::AlignCenter);
    loginLinkLabel->setFont(registerFont);
    registerLayout->addWidget(loginLinkLabel);

    connect(loginLinkLabel, &QLabel::linkActivated, [=]() {
        stackedWidget->setCurrentIndex(0);
        setWindowTitle("摸鱼小助手 - 登录");
    });

    stackedWidget->addWidget(registerPage);

    QString buttonStyle = "QPushButton { background-color: #007bff; color: white; "
                          "padding-left: 10px; padding-right: 10px; padding-top: 5px; padding-bottom: 5px; "
                          "border: 1px solid #8f8f91; border-radius: 8px; } "
                          "QPushButton:hover { background-color: #0056b3; } "
                          "QPushButton:pressed { background-color: #003366; }";
    loginButton->setStyleSheet(buttonStyle);
    registerButton->setStyleSheet(buttonStyle);

    QString textEditStyle = "QLineEdit { border: 1px solid gray; border-radius: 8px; "
                            "padding-left: 10px; padding-right: 10px; padding-top: 5px; padding-bottom: 5px; }"
                            "QLineEdit:focus { border-color: #add8e6; }";
    usernameEdit->setStyleSheet(textEditStyle);
    passwordEdit->setStyleSheet(textEditStyle);
    registerUsernameEdit->setStyleSheet(textEditStyle);
    registerPasswordEdit->setStyleSheet(textEditStyle);
    confirmPasswordEdit->setStyleSheet(textEditStyle);

    loginLayout->setSpacing(8);
    registerLayout->setSpacing(8);
    loginLayout->setContentsMargins(30, 0, 30, 0);
    registerLayout->setContentsMargins(30, 0, 30, 0);

    //registerLabel->setStyleSheet("QLabel { text-decoration: none; }");
    //loginLinkLabel->setStyleSheet("QLabel { text-decoration: none; }");

    // 设置堆叠窗口当前页面为登录界面
    stackedWidget->setCurrentIndex(0);


    QPushButton *skipButton = new QPushButton("跳过 >", this);
    QFont skipFont = skipButton->font();
    skipFont.setPointSize(12);
    skipButton->setFont(skipFont);
    //skipButton->setStyleSheet("color: #3399ff; border: none;");
    skipButton->setStyleSheet("color: gray; border: none;");

    QLabel *copyrightLabel = new QLabel("© 2024 摸鱼小助手", this);
    QFont copyrightFont = copyrightLabel->font();
    copyrightFont.setPointSize(8);
    copyrightLabel->setFont(copyrightFont);
    copyrightLabel->setStyleSheet("color: gray");

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(copyrightLabel);
    bottomLayout->addStretch();
    bottomLayout->addWidget(skipButton);


    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setAlignment(Qt::AlignCenter);
    centralLayout->addWidget(stackedWidget);
    centralLayout->addLayout(bottomLayout);
    centralLayout->setSpacing(20);


    connect(skipButton, &QPushButton::clicked, this, &LoginWindow::skipButtonClicked);
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::login_clicked);
    connect(registerButton, &QPushButton::clicked, this, &LoginWindow::register_clicked);
}

void LoginWindow::skipButtonClicked() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "跳过登录", "跳过登录后将无法使用在线分享、云端同步等功能，且数据存在丢失风险。", QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        save_value("username", "");
        save_value("password", "");
        save_value("token", "");
        MainWindow *mainWindow = new MainWindow();
        mainWindow->show();
        close();
    }
}

void LoginWindow::login_clicked() {
    if (usernameEdit->text() == "") {
        show_warning("登录失败", "用户名不能为空。");
        return;
    }
    if (passwordEdit->text() == "") {
        show_warning("登录失败", "密码不能为空。");
        return;
    }
    loginButton->setEnabled(false);
    Network *n = new Network("https://geomedraw.com/qt/login"); // todo: 内存泄漏？
    n->add_data("username", usernameEdit->text());
    n->add_data("password", passwordEdit->text());
    save_value("username", usernameEdit->text());
    save_value("password", passwordEdit->text());
    n->post();
    connect(n, &Network::succeeded, [=] (QString reply) {
        if (reply == "") {
            show_warning("登录失败", "用户名或密码错误，请检查输入的信息。");
            loginButton->setEnabled(true);
        } else {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(reply.toUtf8());
            if (!jsonDoc.isNull()) {
                QJsonObject jsonObj = jsonDoc.object();
                QString token = jsonObj["token"].toString();
                QString userid = jsonObj["userid"].toString();
                save_value("token", token);
                save_value("userid", userid);
                MainWindow *mainWindow = new MainWindow();
                mainWindow->show();
                close();
            } else {
                qDebug() << "Failed to parse JSON.";
                show_warning("网络错误", "网络连接失败。");
                loginButton->setEnabled(true);
            }
        }
    });
    connect(n, &Network::failed, [=] () {
        show_warning("网络错误", "网络连接失败。");
        loginButton->setEnabled(true);
    });
}

void LoginWindow::register_clicked() {
    if (registerUsernameEdit->text().replace(" ", "") == "") {
        show_warning("注册失败", "用户名不能为空。");
        return;
    }
    if (registerPasswordEdit->text().length() < 6) {
        show_warning("注册失败", "密码长度至少为6位。");
        return;
    }
    if (registerPasswordEdit->text() != confirmPasswordEdit->text()) {
        show_warning("注册失败", "请确保两次输入的密码内容一致。");
        return;
    }
    registerButton->setEnabled(false);
    Network *n = new Network("https://geomedraw.com/qt/register"); // todo: 内存泄漏？
    //QString username = registerUsernameEdit->text();
    //QString password = registerPasswordEdit->text();
    n->add_data("username", registerUsernameEdit->text());
    n->add_data("password", registerPasswordEdit->text());
    n->post();
    connect(n, &Network::succeeded, [=] (QString reply) {
        if (reply != "true") {
            show_warning("注册失败", reply);
            registerButton->setEnabled(true);
        } else {
            show_info("注册成功", "注册成功！");
            usernameEdit->setText(registerUsernameEdit->text());
            passwordEdit->setText(registerPasswordEdit->text());
            stackedWidget->setCurrentIndex(0);
        }
    });
    connect(n, &Network::failed, [=] () {
        show_warning("网络错误", "网络连接失败。");
        registerButton->setEnabled(true);
    });
}

LoginWindow::~LoginWindow()
{
    //qDebug() << "???";
}
