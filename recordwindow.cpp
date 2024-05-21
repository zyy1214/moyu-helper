#include "recordwindow.h"
#include "./ui_recordwindow.h"

#include "record.h"
#include "uitools.h"
#include "data_storage.h"
#include "network.h"

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
    // data->mods.push_back(new Mod(0, QString("跑步{跑步里程}km"), new Formula(QString("跑步里程")), OBTAIN, "跑步"));
    // data->mods.push_back(new Mod(1, QString("做谢慧民{题目数量}道题"), new Formula(QString("题目数量/4")), OBTAIN, "做谢慧民"));
    // data->mods.push_back(new Mod(0, QString("玩原神{游戏时间}分钟（抵扣{抵扣时长}分钟）"), new Formula(QString("max((游戏时间-抵扣时长)/30,0)")), CONSUME, "玩原神"));
    // data->mods.push_back(new Mod(1, QString("做作业"),new Formula(QString("1")), OBTAIN, "做作业"));
    // data->mods.push_back(new Mod(1, QString("测试{x}{y}{z}{w}"),new Formula(QString("(x+y)*(z+w)")), CONSUME, "test"));

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
    Network *n = new Network(this, "https://v1.hitokoto.cn/");
    n->post([] (void *w, QString reply) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply.toUtf8());
        if (!jsonDoc.isNull()) {
            QJsonObject jsonObj = jsonDoc.object();
            QString author = jsonObj["from_who"].toString();
            QString text = "    “" + jsonObj["hitokoto"].toString() + "”";
            RecordWindow *window = (RecordWindow *) w;
            window->ui->quotation->setText(text);
            if (author != "") {
                window->ui->quotation_author->setText("——" + author);
            }
        } else {
            qDebug() << "Failed to parse JSON.";
        }
    }, [] (QMainWindow *w) {

    });
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
    QComboBox *comboBox;
    QVBoxLayout *inputLayout;
    QLabel *preview_result_label;
public:
    EditRecordDialog(RecordWindow *window, Record *record = nullptr, QWidget *parent = nullptr)
        : window(window), record(record), QDialog(parent) {
        qDebug() << record;
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
        comboBox = new QComboBox(this);
        comboBox->setFont(font);
        comboBox->setStyleSheet("font: 12pt Microsoft YaHei UI;");
        comboBox->setEditable(true);
        comboBox->addItem("-");
        for (auto mod : window->data->mods) {
            comboBox->addItem(mod->get_shortname());
        }

        QHBoxLayout *select_mod_layout = new QHBoxLayout();
        select_mod_layout->addWidget(mod_label);
        select_mod_layout->addWidget(comboBox);

        QScrollArea *scrollArea = new QScrollArea(this);
        scrollArea->setWidgetResizable(true);
        scrollArea->setFrameStyle(QFrame::NoFrame);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QWidget *scrollAreaWidget = new QWidget(this);
        inputLayout = new QVBoxLayout(scrollAreaWidget);
        inputLayout->setAlignment(Qt::AlignTop);
        scrollArea->setWidget(scrollAreaWidget);
        connect(comboBox, &QComboBox::currentIndexChanged, this, &EditRecordDialog::updateInputFields);

        preview_result_label = new QLabel("");
        preview_result_label->setFont(font);

        QVBoxLayout *mod_layout = new QVBoxLayout(mod_widget);
        mod_layout->setContentsMargins(30, 20, 30, 20);
        mod_layout->addLayout(select_mod_layout);
        mod_layout->addWidget(scrollArea);
        mod_layout->addWidget(preview_result_label);

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

        direct_layout->addLayout(optionLayout);
        direct_layout->addLayout(textInputLayout);
        direct_layout->addLayout(numberInputLayout);
        //if (!record) direct_layout->addLayout(dateLayout);

        direct_widget->setLayout(direct_layout);
        tabWidget->addTab(direct_widget, "直接添加");
        if (record) {
            if (record->get_class() == DIRECT) {
                tabWidget->setCurrentWidget(direct_widget);
                tabWidget->setTabEnabled(0, false);
            } else {
                tabWidget->setTabEnabled(1, false);
                RecordByMod *rbm = dynamic_cast<RecordByMod *>(record);
                int index = 0;
                QString mod_name = rbm->get_mod()->get_shortname();
                for (int i = 1, len = comboBox->count(); i < len; i++) {
                    if (mod_name == comboBox->itemText(i)) {
                        index = i;
                        break;
                    }
                }
                if (index == 0) {
                    // todo: 如果是被删掉的 mod，...
                }
                comboBox->setCurrentIndex(index);
                updateInputFields(index);
                double *inputs = rbm->get_inputs();
                // todo: input_num 理应是 private 的
                qDebug() << selected_mod->input_num;
                for (int i = 0; i < selected_mod->input_num; i++) {
                    variable_inputs[i]->setText(QString::number(inputs[i]));
                }
            }
        }

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
            dateLayout->addSpacing(20);
            dateLayout->addWidget(dateLabel);
            dateLayout->addWidget(dateEdit);
            dateLayout->addSpacing(20);
            dateLayout->setContentsMargins(0, 5, 0, 5);
        }

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(tabWidget);
        if (!record) layout->addItem(dateLayout);

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
private:
    void clearLayout(QLayout *layout) {
        QLayoutItem *child;
        while ((child = layout->takeAt(0)) != nullptr) {
            if (child->layout() != nullptr) {
                clearLayout(child->layout());
                delete child->layout();
            } else if (child->widget() != nullptr) {
                child->widget()->setParent(nullptr); // This is important to avoid double deletion
                delete child->widget();
            }
            //delete child;
        }
    }

    std::vector<QLineEdit *> variable_inputs;
    Mod *selected_mod;

    double *get_inputs() {
        double *inputs = new double[variable_inputs.size()];
        for (int i = 0, len = variable_inputs.size(); i < len; i++) {
            inputs[i] = variable_inputs[i]->text().toDouble();
        }
        return inputs;
    }

private slots:
    void updateInputFields(int index) {
        if (inputLayout->layout()) {
            clearLayout(inputLayout->layout());
        }

        if (index == 0) {
            selected_mod = nullptr;
            return;
        }
        selected_mod = window->data->mods[index - 1];
        // todo: 把 variable 改成 get_variables
        std::vector<QString> variables = selected_mod->variable;
        //qDebug() << variables;

        variable_inputs.clear();

        QFont font("Microsoft YaHei UI", 12);
        for (const QString &variable : variables) {
            QHBoxLayout *rowLayout = new QHBoxLayout();
            QLabel *label = new QLabel(variable + "：");
            QLineEdit *lineEdit = new QLineEdit();
            lineEdit->setValidator(new QDoubleValidator(lineEdit));
            connect(lineEdit, &QLineEdit::textChanged, this, &EditRecordDialog::variable_edited);
            label->setFont(font);
            lineEdit->setFont(font);
            rowLayout->addWidget(label);
            rowLayout->addWidget(lineEdit);
            inputLayout->addLayout(rowLayout);
            variable_inputs.push_back(lineEdit);
        }

        variable_edited("");
    }

    void variable_edited(const QString &text) {
        double *inputs = get_inputs();
        RecordByMod record(selected_mod, inputs, QDate::currentDate());
        preview_result_label->setText((record.get_type() == OBTAIN ? "将获取 " : "将消耗 ")
                                      + QString::number(record.get_point()) + " 分");
        delete[] inputs;
    }

    void onOKButtonClicked() {
        // todo: 对修改进行合法性校验，包括是否未选中任何 mod 等情况
        enum RECORD_TYPE type = optionRadioButton1->isChecked() ? OBTAIN : CONSUME;
        if (!record) {
            QDate date = dateEdit->date();
            if (tabWidget->currentIndex() == 0) {
                record = new RecordByMod(selected_mod, get_inputs(), date);
            } else {
                record = new RecordDirect(textLineEdit->text(), type, numberLineEdit->text().toInt(), date);
            }
            qDebug() << record;
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
            int d_points = record->get_signed_point();
            if (record->get_class() == BY_MOD) {
                RecordByMod *rbm = dynamic_cast<RecordByMod *>(record);
                if (rbm) {
                    rbm->set_mod(selected_mod);
                    rbm->set_inputs(get_inputs());
                }
            } else {
                RecordDirect *rd = dynamic_cast<RecordDirect *>(record);
                if (rd) {
                    rd->set_name(textLineEdit->text());
                    rd->set_type(type);
                    rd->set_point(numberLineEdit->text().toInt());
                }
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

