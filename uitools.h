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
QPushButton *create_standard_button(QString text, int size);

template<class Function>
QPushButton *create_icon_button(QString icon_name, int size, Function onClick) {
    QPushButton *button = create_icon_button(icon_name, size);
    QObject::connect(button, &QPushButton::clicked, onClick);
    return button;
}

class ConfirmDialog : public QDialog {
    Q_OBJECT
    QMainWindow *window;
public:
    ConfirmDialog(QMainWindow *window, QString title, QString content, QWidget *parent = nullptr);
signals:
    void confirmed();
private slots:
    void onOKButtonClicked();
};

void show_warning(QString title, QString content);
void show_info(QString title, QString content);
// void create_confirm_dialog(QMainWindow *window, QString title, QString content) {
//     ConfirmDialog cd(window, title, content);
//     cd.exec();
// }

void set_text(QLabel *label, const QString &text);

class ConfirmLayout : public QHBoxLayout {
    Q_OBJECT
    QPushButton *okButton;
    QPushButton *cancelButton;
public:
    ConfirmLayout(QWidget *parent = nullptr);
    void setEnabled(bool enabled);
signals:
    void confirmed();
    void canceled();
private slots:
    void on_confirmed() {
        emit confirmed();
    }
    void on_canceled() {
        emit canceled();
    }
};

#endif // UITOOLS_H`
