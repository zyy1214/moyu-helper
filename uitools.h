#ifndef UITOOLS_H
#define UITOOLS_H

#include <QIcon>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QString>

QIcon *get_icon(QString name);
QPushButton *create_icon_button(QString icon_name, int size);

template<class F>
QPushButton *create_icon_button(QString icon_name, int size, F onClick) {
    QPushButton *button = create_icon_button(icon_name, size);
    QObject::connect(button, &QPushButton::clicked, onClick);
    return button;
}

void show_warning(QString title, QString content);
void show_info(QString title, QString content);
template<class F>
void show_confirm(QMainWindow *window, QString title, QString content, F f) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, title, content, QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        f(window);
    }
}

void set_text(QLabel *label, const QString &text);

#endif // UITOOLS_H`
