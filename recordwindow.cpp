#include "recordwindow.h"
#include "./ui_recordwindow.h"

#include "record.h"
#include "uitools.h"
#include "data_storage.h"

#include <QApplication>
#include <QComboBox>
#include <QDateEdit>
#include <QListView>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStandardItem>
#include <QStringListModel>
#include <QListWidgetItem>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QMouseEvent>
#include <QDialog>
#include <QLineEdit>
#include <QRadioButton>
#include <QScrollBar>

void RecordWindow::init_data() {
    load_data(data->records);
    for (auto mr : data->records) {
        for (auto r : *(mr.second)) {
            int point = r->get_signed_point();
            data->total_points += point;
            if (mr.first.daysTo(QDate::currentDate()) <= 6) {
                data->last_week_points += point;
            }
        }
    }
}

void RecordWindow::setup_total_points() {
    if (data->total_points >= 0) {
        ui->info_total_points->setText(QString::number(data->total_points));
        ui->info_total_points->setStyleSheet("color: rgb(23, 112, 228)");
    } else {
        ui->info_total_points->setText("−" + QString::number(-data->total_points));
        ui->info_total_points->setStyleSheet("color: rgb(204, 0, 0)");
    }
    if (data->last_week_points >= 0) {
        ui->last_week_total_points->setText("+" + QString::number(data->last_week_points));
        ui->last_week_total_points->setStyleSheet("color: rgb(23, 112, 228)");
    } else {
        ui->last_week_total_points->setText("−" + QString::number(-data->last_week_points));
        ui->last_week_total_points->setStyleSheet("color: rgb(204, 0, 0)");
    }
}


RecordWindow::RecordWindow(Data *data, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::RecordWindow), data(data)
{
    init_data();
    ui->setupUi(this);
    setup_total_points();
    ui->record_list->setFrameStyle(QFrame::NoFrame);
    ui->record_list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->record_list_area->setFrameStyle(QFrame::NoFrame);
    ui->record_list_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    if (!data->records.empty()) {
        ui->date_selector->setDateRange((*data->records.rbegin()).first, (*data->records.begin()).first);
    } else {
        ui->date_selector->setDateRange(QDate::currentDate(), QDate::currentDate());
    }
    on_option_by_day_pressed();
    setup_records();
}

RecordWindow::~RecordWindow()
{
    qDebug() << "RecordWindow deleted";
    delete ui;
}


class EditRecordDialog : public QDialog {
private:
    QTabWidget *tabWidget;
    QLineEdit *textLineEdit;
    QLineEdit *numberLineEdit;
    QDateEdit *dateEdit;
    QRadioButton *optionRadioButton1;
    QRadioButton *optionRadioButton2;
    Record *record;
    RecordWindow *window;
public:
    EditRecordDialog(RecordWindow *window, Record *record = nullptr, QWidget *parent = nullptr)
        : window(window), record(record), QDialog(parent) {
        setWindowTitle(record ? "修改记录" : "添加记录");
        //setFixedSize(300, 240);

        setStyleSheet("background-color: white;");

        QFont font("Microsoft YaHei UI", 12);

        // 创建 QTabWidget
        tabWidget = new QTabWidget(this);
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setFont(font);

        QWidget *mod_widget = new QWidget();

        QLabel *mod_label = new QLabel("选择模板：", this);
        mod_label->setFont(font);
        mod_label->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
        QComboBox *comboBox = new QComboBox(this);
        comboBox->setFont(font);
        comboBox->setStyleSheet("font: 12pt Microsoft YaHei UI;");
        comboBox->setEditable(true);
        comboBox->addItem("跑步");
        comboBox->addItem("看书");
        comboBox->addItem("背单词");
        comboBox->addItem("玩游戏");
        comboBox->addItem("做谢慧民");
        comboBox->addItem("做算法题");

        QHBoxLayout *select_mod_layout = new QHBoxLayout();
        select_mod_layout->addWidget(mod_label);
        select_mod_layout->addWidget(comboBox);

        QVBoxLayout *mod_layout = new QVBoxLayout(mod_widget);
        mod_layout->addLayout(select_mod_layout);

        mod_widget->setLayout(mod_layout);
        tabWidget->addTab(mod_widget, "从模版添加");

        QWidget *direct_widget = new QWidget();
        // 添加单选按钮和标签
        optionRadioButton1 = new QRadioButton("获取", this);
        optionRadioButton1->setFont(font);
        optionRadioButton2 = new QRadioButton("消耗", this);
        optionRadioButton2->setFont(font);
        optionRadioButton1->setChecked(true);

        QHBoxLayout *optionLayout = new QHBoxLayout();
        //optionLayout->addWidget(optionLabel);
        optionLayout->addWidget(optionRadioButton1);
        optionLayout->addWidget(optionRadioButton2);

        // 添加文本输入框和标签
        QLabel *textLabel = new QLabel("获取/消耗原因：", this);
        textLabel->setFont(font);
        textLineEdit = new QLineEdit(this);
        textLineEdit->setFont(font);

        QHBoxLayout *textInputLayout = new QHBoxLayout();
        textInputLayout->addWidget(textLabel);
        textInputLayout->addWidget(textLineEdit);

        // 添加数字输入框和标签
        QLabel *numberLabel = new QLabel("获取/消耗数量：", this);
        numberLabel->setFont(font);
        numberLineEdit = new QLineEdit(this);
        numberLineEdit->setFont(font);
        QIntValidator *validator = new QIntValidator(0, INT_MAX, this);
        numberLineEdit->setValidator(validator);

        QHBoxLayout *numberInputLayout = new QHBoxLayout();
        numberInputLayout->addWidget(numberLabel);
        numberInputLayout->addWidget(numberLineEdit);


        // 添加日期选择框和标签
        QHBoxLayout *dateLayout;
        if (!record) {
            QLabel *dateLabel = new QLabel("选择日期：", this);
            dateLabel->setFont(font);
            dateLabel->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
            dateEdit = new QDateEdit(this);
            dateEdit->setFont(font);
            dateEdit->setDate(QDate::currentDate());
            dateEdit->setMaximumDate(QDate::currentDate());

            dateLayout = new QHBoxLayout();
            dateLayout->addWidget(dateLabel);
            dateLayout->addWidget(dateEdit);
        }


        if (record) {
            if (record->get_type() == CONSUME) {
                optionRadioButton2->setChecked(true);
            }
            textLineEdit->setText(record->get_display_name());
            numberLineEdit->setText(QString::number(record->get_point()));
        }

        QVBoxLayout *direct_layout = new QVBoxLayout(this);
        direct_layout->setContentsMargins(30, 20, 30, 20);

        optionLayout->setContentsMargins(0, 0, 0, 5);
        textInputLayout->setContentsMargins(0, 5, 0, 5);
        numberInputLayout->setContentsMargins(0, 5, 0, 5);
        if (!record) dateLayout->setContentsMargins(0, 5, 0, 5);

        direct_layout->addLayout(optionLayout);
        direct_layout->addLayout(textInputLayout);
        direct_layout->addLayout(numberInputLayout);
        if (!record) direct_layout->addLayout(dateLayout);

        direct_widget->setLayout(direct_layout);
        tabWidget->addTab(direct_widget, "直接添加");

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(tabWidget);

        // 添加确定和取消按钮
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *okButton = new QPushButton("确定", this);
        okButton->setFont(font);
        QPushButton *cancelButton = new QPushButton("取消", this);
        cancelButton->setFont(font);
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addWidget(okButton);

        buttonLayout->setContentsMargins(0, 5, 0, 5);
        layout->addLayout(buttonLayout);

        // 连接按钮的信号和槽
        connect(okButton, &QPushButton::clicked, this, &EditRecordDialog::onOKButtonClicked);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::close);

        setLayout(layout);
        adjustSize();
    }
private slots:
    void onOKButtonClicked() {
        enum RECORD_TYPE type = optionRadioButton1->isChecked() ? OBTAIN : CONSUME;
        if (!record) {
            QDate date = dateEdit->date();
            record = new RecordDirect(textLineEdit->text(), type, numberLineEdit->text().toInt(), date);
            record->create_uuid();
            record->set_id(db_add_record(record));
            MultipleRecord *daily_record = window->data->records[date];
            if (!daily_record) daily_record = new MultipleRecord;
            daily_record->add_record(record);
            window->data->records[date] = daily_record;

            window->data->total_points += record->get_signed_point();
            if (date.daysTo(QDate::currentDate()) <= 6) {
                window->data->last_week_points += record->get_signed_point();
            }
            // todo: 如果有需要，更改日期选择 widget 的可选择范围
        }
        else {
            RecordDirect *rd = dynamic_cast<RecordDirect *>(record);
            int d_points = record->get_signed_point();
            if (rd) {
                rd->set_name(textLineEdit->text());
                rd->set_type(type);
                rd->set_point(numberLineEdit->text().toInt());
            }
            db_modify_record(record);
            d_points = record->get_signed_point() - d_points;
            window->data->total_points += d_points;
            if (record->get_date().daysTo(QDate::currentDate()) <= 6) {
                window->data->last_week_points += d_points;
            }
        }
        window->setup_total_points();
        window->setup_records(); // todo: 需更改
        close();
    }
};

class DeleteRecordDialog : public QDialog {
private:
    Record *record;
    RecordWindow *window;
public:
    DeleteRecordDialog(RecordWindow *window, Record *record, QWidget *parent = nullptr)
        : window(window), record(record), QDialog(parent) {
        //qDebug() << window;
        setWindowTitle("删除记录");
        setFixedSize(300, 120);

        setStyleSheet("background-color: white;");

        QFont font("Microsoft YaHei UI", 12);

        // 添加文本输入框和标签
        QLabel *textLabel = new QLabel("确定删除该条记录吗？", this);
        textLabel->setFont(font);


        // 布局管理器
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(textLabel);

        // 添加确定和取消按钮
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *okButton = new QPushButton("确定", this);
        okButton->setFont(font);
        QPushButton *cancelButton = new QPushButton("取消", this);
        cancelButton->setFont(font);
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addWidget(okButton);
        layout->addLayout(buttonLayout);

        // 连接按钮的信号和槽
        connect(okButton, &QPushButton::clicked, this, &DeleteRecordDialog::onOKButtonClicked);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::close);

        setLayout(layout);
    }
    ~DeleteRecordDialog() {
        qDebug() << "DeleteRecordDialog deleted";
    }
private slots:
    void onOKButtonClicked() {
        MultipleRecord *mr = window->data->records[record->get_date()];
        for (int i = 0; i < mr->size(); i++) {
            if (record->get_id() == (*mr)[i]->get_id()) {
                mr->delete_record(i);
                int d_points = record->get_signed_point();
                window->data->total_points -= d_points;
                if (record->get_date().daysTo(QDate::currentDate()) <= 6) {
                    window->data->last_week_points -= d_points;
                }
                break;
            }
        }
        if (mr->size() == 0) {
            window->data->records.erase(record->get_date());
        }
        db_delete_record(record);
        window->setup_total_points();
        window->setup_records(); // todo: 需更改
        close();
    }
};

class RecordItem : public QWidget {
private:
    RecordWindow *window;
public:
    RecordItem(RecordWindow *window, Record &record, QWidget *parent = nullptr)
        : window(window), QWidget(parent) {
        //qDebug() << "RecordItem created" << window << record.get_point() << this;
        setStyleSheet("background-color: rgba(255, 255, 255, 100)");
        QFont font("Microsoft YaHei UI", 18);
        QSpacerItem *spacer = new QSpacerItem(40, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
        QLabel *label_name = new QLabel(record.get_display_name());
        label_name->setFont(font);
        QLabel *label_point = new QLabel((record.get_type() == RECORD_TYPE::OBTAIN ? "+" : "−") + QString::number(record.get_point()));
        label_point->setFont(font);
        label_point->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
        QSpacerItem *spacer1 = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
        QPushButton *button_edit = create_icon_button("edit", 32, [&, window] () {
            EditRecordDialog *dialog = new EditRecordDialog(window, &record);
            dialog->exec();
        });
        QPushButton *button_delete = create_icon_button("delete", 32, [&, window] () {
            DeleteRecordDialog *dialog = new DeleteRecordDialog(window, &record);
            dialog->exec();
        });

        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->addItem(spacer);
        layout->addWidget(label_name);
        layout->addWidget(label_point);
        layout->addItem(spacer1);
        layout->addWidget(button_edit);
        layout->addWidget(button_delete);
        setFixedHeight(50);

        // todo: 实现类似 Android 中 ellipsize=end 的效果
        //set_text(label_name, record.get_display_name());
    }
    RecordItem (const RecordItem &ri) {
        //qDebug() << "RecordItem copy constructor called";
    }

    ~RecordItem() {
        //qDebug() << window;
        //qDebug() << "RecordItem deleted" << this;
    }
};

class DateItem : public QWidget {
public:
    DateItem(const QDate &date, QWidget *parent = nullptr) : QWidget(parent) {
        setStyleSheet("background-color: rgba(255, 255, 255, 100)");
        QFont font("Microsoft YaHei UI", 18);
        font.setBold(true);
        QSpacerItem *spacer = new QSpacerItem(25, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);

        QString weekday[7] = {"一", "二", "三", "四", "五", "六", "日"};
        QLabel *label_date = new QLabel(QString::number(date.year()) + " 年 " + QString::number(date.month()) + " 月 " + QString::number(date.day()) + " 日    星期" + weekday[date.dayOfWeek() - 1]);
        label_date->setFont(font);
        //label_date->setMargin(10);

        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->addItem(spacer);
        layout->addWidget(label_date);
        setFixedHeight(50);
    }
};

class WeekItem : public QWidget {
public:
    WeekItem(const QDate &date, QWidget *parent = nullptr) : QWidget(parent) {
        setStyleSheet("background-color: rgba(255, 255, 255, 100)");
        QFont font("Microsoft YaHei UI", 18);
        font.setBold(true);
        QSpacerItem *spacer = new QSpacerItem(25, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);

        QDate last = date.addDays(6);

        QLabel *label_date = new QLabel(QString::number(date.year()) + " 年 " + QString::number(date.month()) + " 月 " + QString::number(date.day()) + " 日 – "
                                        + QString::number(last.year()) + " 年 " + QString::number(last.month()) + " 月 " + QString::number(last.day()) + " 日");
        label_date->setFont(font);

        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->addItem(spacer);
        layout->addWidget(label_date);
        setFixedHeight(50);
    }
};

class MonthItem : public QWidget {
public:
    MonthItem(int y, int m, QWidget *parent = nullptr) : QWidget(parent) {
        setStyleSheet("background-color: rgba(255, 255, 255, 100)");
        QFont font("Microsoft YaHei UI", 18);
        font.setBold(true);
        QSpacerItem *spacer = new QSpacerItem(25, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);

        QLabel *label_date = new QLabel(QString::number(y) + " 年 " + QString::number(m) + " 月");
        label_date->setFont(font);

        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->addItem(spacer);
        layout->addWidget(label_date);
        setFixedHeight(50);
    }
};

class YearItem : public QWidget {
public:
    YearItem(int y, QWidget *parent = nullptr) : QWidget(parent) {
        setStyleSheet("background-color: rgba(255, 255, 255, 100)");
        QFont font("Microsoft YaHei UI", 18);
        font.setBold(true);
        QSpacerItem *spacer = new QSpacerItem(25, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);

        QLabel *label_date = new QLabel(QString::number(y) + " 年");
        label_date->setFont(font);

        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->addItem(spacer);
        layout->addWidget(label_date);
        setFixedHeight(50);
    }
};


int display_option = 1; // 1: 天；2: 周；3: 月；4: 年

void RecordWindow::setup_records() {
    QWidget *scrollWidget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

    switch(display_option) {
        case 1: {
            //int i = 0;
            for (auto &date : data->records) {
                layout->addWidget(new DateItem(date.first));
                //i++;
                for (auto &record : *date.second) {
                    //qDebug() << "here";
                    auto *item = new RecordItem(this, *record);
                    layout->addWidget(item);
                    //i++;
                }
            }
            break;
        }
        case 2: {
            QDate current = QDate::currentDate().addDays(1 - QDate::currentDate().dayOfWeek());
            layout->addWidget(new WeekItem(current));
            auto i = data->records.begin();
            do {
                if ((*i).first < current) {
                    current = (*i).first.addDays(1 - (*i).first.dayOfWeek());
                    layout->addWidget(new WeekItem(current));
                }
            } while((++i) != data->records.end());
            break;
        }
        case 3: {
            int y = QDate::currentDate().year(), m = QDate::currentDate().month();
            layout->addWidget(new MonthItem(y, m));
            auto i = data->records.begin();
            do {
                if ((*i).first < QDate{y, m, 1}) {
                    y = (*i).first.year();
                    m = (*i).first.month();
                    layout->addWidget(new MonthItem(y, m));
                }
            } while((++i) != data->records.end());
            break;
        }
        case 4: {
            int y = QDate::currentDate().year();
            layout->addWidget(new YearItem(y));
            auto i = data->records.begin();
            do {
                if ((*i).first < QDate{y, 1, 1}) {
                    y = (*i).first.year();
                    layout->addWidget(new YearItem(y));
                }
            } while((++i) != data->records.end());
            break;
        }
    }
    layout->addItem(spacer);

    scrollWidget->setLayout(layout);
    QScrollArea *scrollArea = ui->record_list_area;
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);
    //scrollArea->verticalScrollBar()->setMinimum(1);
    //scrollArea->verticalScrollBar()->setMaximum(i);
}


void RecordWindow::on_add_record_clicked() {
    EditRecordDialog dialog(this);
    dialog.exec();
}


void RecordWindow::on_date_selector_selectionChanged() {
    if (data->records.empty()) return;
    QDate selected = ui->date_selector->selectedDate();
    auto it = data->records.lower_bound(selected);
    int value = 0;
    for (auto i = data->records.begin(); i != it; i++) {
        value += (*i).second->size() + 1;
        //if (i == it) break;
    }
    QScrollBar *verticalScrollBar = ui->record_list_area->verticalScrollBar();
    verticalScrollBar->setValue(56 * value - 6);
    //std::cout << value << std::endl;
}



void RecordWindow::clear_option_choose() {
    ui->option_by_day->setStyleSheet("border: none; outline: none; padding: 5;");
    ui->option_by_week->setStyleSheet("border: none; outline: none; padding: 5;");
    ui->option_by_month->setStyleSheet("border: none; outline: none; padding: 5;");
    ui->option_by_year->setStyleSheet("border: none; outline: none; padding: 5;");
}

void RecordWindow::on_option_by_day_pressed() {
    clear_option_choose();
    ui->option_by_day->setStyleSheet("border: none; outline: none; padding: 5; background-color: rgb(192, 220, 243);");
    display_option = 1;
    setup_records();
}
void RecordWindow::on_option_by_week_clicked() {
    clear_option_choose();
    ui->option_by_week->setStyleSheet("border: none; outline: none; padding: 5; background-color: rgb(192, 220, 243);");
    display_option = 2;
    setup_records();
}
void RecordWindow::on_option_by_month_clicked() {
    clear_option_choose();
    ui->option_by_month->setStyleSheet("border: none; outline: none; padding: 5; background-color: rgb(192, 220, 243);");
    display_option = 3;
    setup_records();
}
void RecordWindow::on_option_by_year_clicked() {
    clear_option_choose();
    ui->option_by_year->setStyleSheet("border: none; outline: none; padding: 5; background-color: rgb(192, 220, 243);");
    display_option = 4;
    setup_records();
}

