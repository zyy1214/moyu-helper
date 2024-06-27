#include <QFontDatabase>
#include <QFontMetrics>
#include <QLabel>
#include <QIcon>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QStyle>
#include <unordered_map>

#include "uitools.h"


ConfirmLayout::ConfirmLayout(QWidget *parent): QHBoxLayout(parent) {
    okButton = new QPushButton("确定", parent);
    okButton->setStyleSheet("color: black; font-size: 14px;"
                            "padding-left: 25px; padding-right: 25px; padding-top: 6px; padding-bottom: 6px;");
    okButton->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
    cancelButton = new QPushButton("取消", parent);
    cancelButton->setStyleSheet("color: black; font-size: 14px;"
                                "padding-left: 25px; padding-right: 25px; padding-top: 6px; padding-bottom: 6px;");
    cancelButton->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
    setAlignment(Qt::AlignRight);
    addWidget(cancelButton);
    addSpacing(8);
    addWidget(okButton);
    addSpacing(25);
    connect(okButton, &QPushButton::clicked, this, &ConfirmLayout::on_confirmed);
    connect(cancelButton, &QPushButton::clicked, this, &ConfirmLayout::on_canceled);
};
void ConfirmLayout::setEnabled(bool enabled) {
    okButton->setEnabled(enabled);
    cancelButton->setEnabled(enabled);
}

ConfirmDialog::ConfirmDialog(QMainWindow *window, QString title, QString content, QWidget *parent)
    : window(window), QDialog(parent) {

    setWindowTitle(title);
    setWindowIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));

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

void ConfirmDialog::closeEvent(QCloseEvent *event) {
    emit canceled();
}

void ConfirmDialog::onOKButtonClicked() {
    emit confirmed();
    close();
}

std::unordered_map<QString, QIcon *> icons;
QIcon *get_icon(QString name) {
    auto it = icons.find(name);
    QIcon *icon;
    if (it == icons.end()) {
        icon = new QIcon(name);
        icons[name] = icon;
    } else {
        icon = it->second;
    }
    return icon;
}

std::unordered_map<QString, QString> icon_map; // 记录特定名称的 icon 在字体文件中存放的位置
bool icon_map_inited = false;

void init_icon_map() {
    if (!icon_map_inited) {
        icon_map["delete"] = "D";
        icon_map["edit"] = "E";
        icon_map["image"] = "I";
        icon_map["quit"] = "Q";
        icon_map["refresh"] = "R";
        icon_map["dark"] = "d";
        icon_map["light"] = "l";
        icon_map_inited = true;
    }
}

void setup_icon_button(QPushButton *button, QString icon_name, int size, QString color) {
    init_icon_map();

    if (icon_map.find(icon_name) != icon_map.end()) {
        int fontId = QFontDatabase::addApplicationFont(":/fonts/icons");
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont customFont(fontFamily);
        customFont.setPixelSize(size);
        button->setText(icon_map[icon_name]);
        button->setFont(customFont);
        button->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
        button->setStyleSheet("QPushButton { border: none; outline: none; color: " + color + "; }");
        return;
    }

    button->setIcon(*get_icon(":/images/" + icon_name));
    button->setIconSize(QSize(size, size));
    button->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
    button->setStyleSheet("QPushButton { border: none; outline: none; }");
}

QPushButton *create_icon_button(QString icon_name, int size) {
    QPushButton *button = new QPushButton();
    setup_icon_button(button, icon_name, size);
    return button;
}

QPushButton *create_standard_button(QString text, int size) {
    QPushButton *button = new QPushButton(text);
    button->setStyleSheet("font-size: " + QString::number(size) + "px;"
                          "padding-left: 15px; padding-right: 15px; padding-top: 6px; padding-bottom: 6px;");
    button->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
    return button;
}

class InfoDialog : public QDialog {
public:
    InfoDialog(QString title, QString content, int dialog_type, QWidget *parent = nullptr)
        : QDialog(parent) {

        setWindowTitle(title);
        setWindowIcon(dialog_type == 1 ? style()->standardIcon(QStyle::SP_MessageBoxInformation)
                                       : style()->standardIcon(QStyle::SP_MessageBoxWarning));

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
        iconLabel->setPixmap(style()->standardPixmap(dialog_type == 1 ? QStyle::SP_MessageBoxInformation : QStyle::SP_MessageBoxWarning)
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
        layout->addSpacing(30);

        // 添加确定和取消按钮
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *okButton = new QPushButton("确定", this);
        okButton->setStyleSheet("color: black; font-size: 14px;"
                                "padding-left: 25px; padding-right: 25px; padding-top: 6px; padding-bottom: 6px;");
        okButton->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);

        buttonLayout->setAlignment(Qt::AlignRight);
        buttonLayout->addWidget(okButton);
        buttonLayout->addSpacing(25);

        layout->addLayout(buttonLayout);
        layout->addSpacing(20);


        connect(okButton, &QPushButton::clicked, this, &QDialog::close);

        setLayout(layout);
    }
};

void show_warning(QString title, QString content) {
    // QMessageBox::warning(nullptr, title, content, QMessageBox::Ok);
    InfoDialog *dialog = new InfoDialog(title, content, 2);
    dialog->exec();
}
void show_info(QString title, QString content) {
    // QMessageBox::information(nullptr, title, content, QMessageBox::Ok);
    InfoDialog *dialog = new InfoDialog(title, content, 1);
    dialog->exec();
}

QString get_elliptic_text(const QString &text, const QFont &font, int maxWidth) {
    QFontMetrics fontMetrics(font);
    QRect boundingRect = fontMetrics.boundingRect(text);
    int textWidth = boundingRect.width();
    if (textWidth <= maxWidth) {
        return text;
    } else {
        QString ellipsizedText = text;
        int ellipsisWidth = fontMetrics.boundingRect("...").width();
        while (fontMetrics.boundingRect(ellipsizedText).width() + ellipsisWidth > maxWidth) {
            ellipsizedText.chop(1);
        }
        ellipsizedText.append("...");
        return ellipsizedText;
    }
}

void set_text(QLabel *label, const QString &text) {
    int maxWidth = label->width();
    label->setText(get_elliptic_text(text, label->font(), maxWidth));
}
