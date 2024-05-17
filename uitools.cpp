#include <QFontDatabase>
#include <QFontMetrics>
#include <QLabel>
#include <QIcon>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <unordered_map>

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
        icon_map_inited = true;
    }
}

QPushButton *create_icon_button(QString icon_name, int size) {
    init_icon_map();

    QPushButton *button = new QPushButton();

    if (icon_map.find(icon_name) != icon_map.end()) {
        int fontId = QFontDatabase::addApplicationFont(":/fonts/icons");
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont customFont(fontFamily);
        customFont.setPixelSize(size);
        button->setText(icon_map[icon_name]);
        button->setFont(customFont);
        button->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
        button->setStyleSheet("QPushButton { border: none; outline: none; }");
        return button;
    }

    button->setIcon(*get_icon(":/images/" + icon_name));
    button->setIconSize(QSize(size, size));
    button->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
    button->setStyleSheet("QPushButton { border: none; outline: none; }");
    return button;
}

void show_warning(QString title, QString content) {
    QMessageBox::warning(nullptr, title, content, QMessageBox::Ok);
}
void show_info(QString title, QString content) {
    QMessageBox::information(nullptr, title, content, QMessageBox::Ok);
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
