#ifndef UITOOLS_H
#define UITOOLS_H

#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QStyle>
#include <QVBoxLayout>

QIcon *get_icon(QString name);
void setup_icon_button(QPushButton *button, QString icon_name, int size, QString color = "#000000");
QPushButton *create_icon_button(QString icon_name, int size);

template<class Function>
QPushButton *create_icon_button(QString icon_name, int size, Function onClick) {
    QPushButton *button = create_icon_button(icon_name, size);
    QObject::connect(button, &QPushButton::clicked, onClick);
    return button;
}

template<class Function>
class ConfirmDialog : public QDialog {
    QMainWindow *window;
    Function f;
public:
    ConfirmDialog(QMainWindow *window, QString title, QString content, Function f, QWidget *parent = nullptr)
        : window(window), f(f), QDialog(parent) {

        setWindowTitle(title);
        // setWindowIcon(QIcon());

        // Qt::WindowFlags flags = windowFlags();
        // flags &= ~Qt::WindowTitleHint;
        // flags |= Qt::CustomizeWindowHint;
        // setWindowFlags(flags);

        setFixedWidth(400);

        setPalette(QPalette(QColor(Qt::white)));

        QHBoxLayout *titleLayout = new QHBoxLayout();
        QLabel *titleLabel = new QLabel(title, this);
        titleLabel->setWordWrap(true);
        titleLabel->setStyleSheet("font-weight: bold; font-size: 24px;");
        titleLayout->addSpacing(25);
        titleLayout->addWidget(titleLabel);
        titleLayout->addSpacing(25);

        QHBoxLayout *textLayout = new QHBoxLayout();
        textLayout->setAlignment(Qt::AlignTop);
        QLabel *iconLabel = new QLabel("", this);
        iconLabel->setPixmap(style()->standardPixmap(QStyle::SP_MessageBoxQuestion)
                                 .scaled(36, 36, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        iconLabel->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
        QLabel *textLabel = new QLabel(content, this);
        textLabel->setWordWrap(true);
        textLabel->setStyleSheet("font-size: 18px;");
        textLayout->addSpacing(25);
        textLayout->addWidget(iconLabel);
        textLayout->addSpacing(20);
        textLayout->addWidget(textLabel);
        textLayout->addSpacing(25);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addSpacing(10);
        layout->addItem(titleLayout);
        layout->addSpacing(15);
        layout->addItem(textLayout);
        layout->addSpacing(35);

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
        buttonLayout->addSpacing(25);

        layout->addLayout(buttonLayout);
        layout->addSpacing(25);


        connect(okButton, &QPushButton::clicked, this, &ConfirmDialog::onOKButtonClicked);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::close);

        setLayout(layout);
    }
private slots:
    void onOKButtonClicked() {
        f(window);
        close();
    }
};

void show_warning(QString title, QString content);
void show_info(QString title, QString content);
template<class Function>
void show_confirm(QMainWindow *window, QString title, QString content, Function f) {
    ConfirmDialog<Function> cd(window, title, content, f);
    cd.exec();
    // QMessageBox::StandardButton reply;
    // reply = QMessageBox::question(nullptr, title, content, QMessageBox::Yes|QMessageBox::No);

    // if (reply == QMessageBox::Yes) {
    //     f(window);
    // }
}

void set_text(QLabel *label, const QString &text);

#endif // UITOOLS_H`
