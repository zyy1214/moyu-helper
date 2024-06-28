#include "modwindow.h"
#include "ui_modwindow.h"
#include "mod.h"
#include "uitools.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include <QSpacerItem>
#include <bits/stdc++.h>
#include <QScrollArea>
#include <QSizePolicy>
#include <QLabel>
#include <QFont>
#include <QPushButton>
#include <QIcon>
#include <QDialog>
#include <QLineEdit>
#include <QButtonGroup>
#include <QRadioButton>
#include <QTimer>
#include <QMouseEvent>

#include "data_storage.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>

class MyLineEdit : public QLineEdit {
protected:
    void keyPressEvent(QKeyEvent *e) override {
        // 检查按下的键是否是 Enter 键
        if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
            // 处理 Enter 键
            QLineEdit::returnPressed();
            return ;
        } else {
            // 对于其他按键，调用父类的 keyPressEvent
            QLineEdit::keyPressEvent(e);
        }
    }
};


class tags_dialog : public QDialog { //标签dialog
public:
    ModWindow *window;
    tags_dialog(ModWindow *window, Mod *id,QWidget *parent = nullptr)
        :window(window), aa(id) ,QDialog(parent) {
        setWindowTitle("Label Dialog");
        setupUI();
    }


private:
    bool flag=0;
    Mod *aa;
    void setupUI() {
        qDebug()<<"3"<<aa->labels;
        QFont font("Microsoft YaHei UI", 16);//字体
        delete layout();
        QVBoxLayout* mainLayout = new QVBoxLayout();
        setLayout(mainLayout);
        QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
        QVBoxLayout* labelsLayout = new QVBoxLayout();
        QScrollArea* scrollArea = new QScrollArea();
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        scrollArea->setStyleSheet("background-color: #eeeeee;");

        // 设置对话框的透明度
        this->setStyleSheet("background-color: rgba(255, 255, 255, 188);");

        // 确保滚动区域的背景也是透明的
        //scrollArea->setStyleSheet("background-color: transparent;");

        // 设置滚动区域
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(new QWidget());
        scrollArea->widget()->setLayout(labelsLayout);


        for (int i = 0; i < (aa->labels).size(); i++) {
            QHBoxLayout *label_layout=new QHBoxLayout;
            QLabel *label = new QLabel(aa->labels[i]);
            label->setFont(font);
            //删除更改按钮
            QPushButton *delete_button=create_icon_button("delete", 24,[=](){
                window->delete_label(aa, label->text());
                setupUI();
            });
            label_layout->addWidget(label);
            label_layout->addWidget(delete_button);
            labelsLayout->addLayout(label_layout);
            label->setStyleSheet("color: white; background-color: rgba(0, 0, 255, 188); border-radius: 10px");
            label->setAlignment(Qt::AlignCenter);
        }
        if(flag==0)
        {
            QPushButton *addButton = create_icon_button("plus",24,[=]{
                flag=1;
                setupUI();
            });
            labelsLayout->addWidget(addButton);
        }
        else
        {
            QHBoxLayout *addd= new QHBoxLayout;
            MyLineEdit *add_name= new MyLineEdit();
            add_name->setFont(font);

            QPushButton *addButton = create_icon_button("check",24,[=]{
                flag=0;
                QString addtag=add_name->text();
                if(addtag!=""&& std::find((aa->labels).begin(),(aa->labels).end(),addtag)==(aa->labels).end())
                {
                    window->add_label(aa, addtag);
                }
                setupUI();
            });

            //回车添加tags
            disconnect(add_name, &QLineEdit::returnPressed, nullptr, nullptr);
            //disconnect(add_name, &QLineEdit::keyPressEvent,nullptr,nullptr);
            connect(add_name, &QLineEdit::returnPressed,this, [this,add_name](){
                flag=0;
                QString addtag=add_name->text();
                if(addtag!=""&& std::find((aa->labels).begin(),(aa->labels).end(),addtag)==(aa->labels).end())
                {
                    window->add_label(aa, addtag);
                }
                qDebug()<<"1"<<aa->labels;
                setupUI();
                qDebug()<<"2"<<aa->labels;
            });


            addd->addWidget(add_name);
            addd->addWidget(addButton);
            labelsLayout->addLayout(addd);
        }
        labelsLayout->addItem(spacer);

        mainLayout->addWidget(scrollArea);
    }


};


class TagButton : public QPushButton { //标签button

public:
    ModWindow *window;
    TagButton(ModWindow *window, Mod *id,QWidget *parent = nullptr)
        : window(window), idd(id), QPushButton(parent) {
        connect(&hideTimer, &QTimer::timeout, this, &TagButton::hideDialog);
        installEventFilter(this);
    }

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (obj == dialog) {
            if (event->type() == QEvent::Enter) {
                cancelHideDialog();
            } else if (event->type() == QEvent::Leave) {
                hideDialogAfterDelay();
            }
        } else if (event->type() == QEvent::Enter) {
            if (!dialog) {
                showDialog();
            }
            cancelHideDialog();
        } else if (event->type() == QEvent::Leave) {
            hideDialogAfterDelay();
        } else if (event->type() == QEvent::MouseMove) {
            cancelHideDialog();
        }
        return QPushButton::eventFilter(obj, event);
    }

private:
    Mod *idd;
    void showDialog() {
        // 创建对话框并设置布局
        dialog = new tags_dialog(window, idd,this);
        dialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
        // QVBoxLayout *layout = new QVBoxLayout(dialog);

        // // 添加标签到对话框中
        // QLabel *label1 = new QLabel("标签1", dialog);
        // QLabel *label2 = new QLabel("标签2", dialog);
        // layout->addWidget(label1);
        // layout->addWidget(label2);

        // 设置对话框位置
        dialog->setFixedSize(200, 200);
        //dialog->move(pos().x(),pos().y()+size().height()+80);
        QPoint newPos = this->mapToGlobal(QPoint(0, this->size().height()));
        dialog->move(newPos.x()-100, newPos.y() + 10);

        // 显示对话框
        dialog->show();

        // 安装对话框的事件过滤器
        dialog->installEventFilter(this);
    }

    void hideDialogAfterDelay() {
        hideTimer.start(1000);
    }

    void cancelHideDialog() {
        hideTimer.stop();
    }

private slots:
    void hideDialog() {
        if (dialog) {
            dialog->hide();
            delete dialog;
            dialog = nullptr;
        }
    }

private:
    QDialog *dialog = nullptr;
    QTimer hideTimer;
};


//添加对话框
class AddDialog : public QDialog {

public:
    ModWindow *window;
    AddDialog(ModWindow *window, QWidget *parent = nullptr)
        : window(window), QDialog(parent) {

        QFont font("Microsoft YaHei UI", 12);//字体

        setPalette(QPalette(QColor(Qt::white)));


        setWindowTitle(QString("添加模板"));
        setFixedSize(450, 300);

        // 创建Radio button组
        QRadioButton *consumeButton = new QRadioButton("消耗");
        consumeButton->setFont(font);
        QRadioButton *obtainButton = new QRadioButton("获取");
        obtainButton->setFont(font);
        obtainButton->setChecked(true);

        // 将Radio button添加到按钮组
        buttonGroup.addButton(obtainButton);
        buttonGroup.addButton(consumeButton);

        QHBoxLayout *buttonlayout=new QHBoxLayout;
        buttonlayout->addWidget(consumeButton);
        buttonlayout->addWidget(obtainButton);

        // 创建文本输入框
        QLabel *label_shortname=new QLabel("请输入模板名称：");
        label_shortname->setFont(font);
        QLabel *label_name=new QLabel("请输入模板内容：");
        label_name->setFont(font);
        QLabel *label_formula=new QLabel("请输入公式：");
        label_formula->setFont(font);
        QPushButton *formula_help = create_icon_button("info", 18);
        connect(formula_help, &QPushButton::clicked, [] () {
            show_info("公式", "您可以在模版内容中用大括号定义变量，然后在公式中使用。例如：您可以设置模版为“玩游戏{x}小时”，公式内容为 max(x^2,x*10).");
        });
        QLineEdit *lineEdit_shortname = new QLineEdit();
        lineEdit_shortname->setFont(font);
        QLineEdit *lineEdit_name = new QLineEdit();
        lineEdit_name->setFont(font);
        QLineEdit *lineEdit_formula=new QLineEdit;
        lineEdit_formula->setFont(font);
        QHBoxLayout *input_shortname=new QHBoxLayout;
        QHBoxLayout *input_name=new QHBoxLayout;
        QHBoxLayout *input_formula=new QHBoxLayout;
        input_shortname->addWidget(label_shortname);
        input_shortname->addWidget(lineEdit_shortname);
        input_name->addWidget(label_name);
        input_name->addWidget(lineEdit_name);
        input_formula->addWidget(label_formula);
        input_formula->addWidget(lineEdit_formula);
        input_formula->addWidget(formula_help);

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



        QHBoxLayout *reallayout = new QHBoxLayout;
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addLayout(buttonlayout);
        layout->addLayout(input_shortname);
        layout->addLayout(input_name);
        layout->addLayout(input_formula);
        layout->addSpacing(10);
        layout->addLayout(buttonLayout);


        reallayout->addItem(new QSpacerItem(30, 30, QSizePolicy::Fixed, QSizePolicy::Minimum));
        reallayout->addLayout(layout);
        reallayout->addItem(new QSpacerItem(30, 30, QSizePolicy::Fixed, QSizePolicy::Minimum));

        setLayout(reallayout);

        // 连接确定按钮的点击事件
        connect(okButton, &QPushButton::clicked, [=]() {
            int button_type=(obtainButton->isChecked()) ? 1 : 2;
            QString text_name = lineEdit_name->text();
            QString text_shortname = lineEdit_shortname->text();
            QString text_formula = lineEdit_formula->text();

            Formula bb(text_formula);
            //qDebug()<<"here";
            Mod* mod = new Mod(0,text_name,&bb, button_type == 1 ? OBTAIN : CONSUME, text_shortname);
            //qDebug()<<"here1";
            if(mod->name_legal())
            {
                window->add_mod(text_name,new Formula(text_formula), button_type == 1 ? OBTAIN : CONSUME, text_shortname);
                close();
            }
            else
            {
                show_warning("添加模板", "请检查模板内容输入。");
            }
            delete mod;
        });
        connect(cancelButton, &QPushButton::clicked,[=](){
            close();
        });
    }

private:
    QButtonGroup buttonGroup;
};

//更改对话框
class ChangeDialog : public QDialog {

public:
    ModWindow *window;
    ChangeDialog(ModWindow *window, Mod* x,QWidget *parent = nullptr)
        : window(window), before_mod(x), QDialog(parent) {

        setPalette(QPalette(QColor(Qt::white)));

        QFont font("Microsoft YaHei UI", 12);//字体


        setWindowTitle(QString("添加模板"));
        setFixedSize(460, 350);


        // 创建Radio button组
        QRadioButton *yeschangeButton = new QRadioButton("是");
        yeschangeButton->setFont(font);
        yeschangeButton->setChecked(true);
        QRadioButton *nochangeButton = new QRadioButton("否");
        nochangeButton->setFont(font);
        QLabel *label1=new QLabel("是否更新之前的记录：");
        label1->setFont(font);

        // 将Radio button添加到按钮组
        buttonGroup1.addButton(yeschangeButton);
        buttonGroup1.addButton(nochangeButton);

        QHBoxLayout *buttonlayout1=new QHBoxLayout;
        buttonlayout1->addWidget(label1);
        buttonlayout1->addWidget(yeschangeButton);
        buttonlayout1->addWidget(nochangeButton);



        // 创建Radio button组
        QRadioButton *consumeButton = new QRadioButton("消耗");
        consumeButton->setFont(font);
        QRadioButton *obtainButton = new QRadioButton("获取");
        obtainButton->setFont(font);
        if(x->type==1)
            obtainButton->setChecked(true);
        else
            consumeButton->setChecked(true);


        // 将Radio button添加到按钮组
        buttonGroup.addButton(obtainButton);
        buttonGroup.addButton(consumeButton);

        QHBoxLayout *buttonlayout=new QHBoxLayout;
        buttonlayout->addWidget(consumeButton);
        buttonlayout->addWidget(obtainButton);

        // 创建文本输入框
        QLabel *label_shortname=new QLabel("请输入模板名称：");
        label_shortname->setFont(font);
        QLabel *label_name=new QLabel("请输入模板内容：");
        label_name->setFont(font);
        QLabel *label_formula=new QLabel("请输入公式：");
        label_formula->setFont(font);
        QPushButton *formula_help = create_icon_button("info", 18);
        connect(formula_help, &QPushButton::clicked, [] () {
            show_info("公式", "您可以在模版内容中用大括号定义变量，然后在公式中使用。例如：您可以设置模版为“玩游戏{x}小时”，公式内容为 max(x^2,x*10).");
        });
        QLineEdit *lineEdit_shortname = new QLineEdit();
        lineEdit_shortname->setText(x->get_shortname());
        lineEdit_shortname->setFont(font);
        QLineEdit *lineEdit_name = new QLineEdit();
        lineEdit_name->setText(x->get_name());
        lineEdit_name->setFont(font);
        QLineEdit *lineEdit_formula=new QLineEdit;
        lineEdit_formula->setText(x->get_fun());
        lineEdit_formula->setFont(font);
        QHBoxLayout *input_shortname=new QHBoxLayout;
        QHBoxLayout *input_name=new QHBoxLayout;
        QHBoxLayout *input_formula=new QHBoxLayout;
        input_shortname->addWidget(label_shortname);
        input_shortname->addWidget(lineEdit_shortname);
        input_name->addWidget(label_name);
        input_name->addWidget(lineEdit_name);
        input_formula->addWidget(label_formula);
        input_formula->addWidget(lineEdit_formula);
        input_formula->addWidget(formula_help);

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



        QHBoxLayout *reallayout = new QHBoxLayout;
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addLayout(buttonlayout1);
        layout->addLayout(buttonlayout);
        layout->addLayout(input_shortname);
        layout->addLayout(input_name);
        layout->addLayout(input_formula);
        layout->addSpacing(10);
        layout->addLayout(buttonLayout);

        reallayout->addItem(new QSpacerItem(30, 30, QSizePolicy::Fixed, QSizePolicy::Minimum));
        reallayout->addLayout(layout);
        reallayout->addItem(new QSpacerItem(30, 30, QSizePolicy::Fixed, QSizePolicy::Minimum));

        setLayout(reallayout);

        // 连接确定按钮的点击事件
        connect(okButton, &QPushButton::clicked, [=]() {
            int change_type=(yeschangeButton->isChecked())? 1: 0;
            int button_type=(obtainButton->isChecked()) ? 1 : 2;
            QString text_name = lineEdit_name->text();
            QString text_formula = lineEdit_formula->text();
            QString text_shortname=lineEdit_shortname->text();

            Formula bb(text_formula);
            Mod* aaa=new Mod(0,text_name,&bb, button_type == 1 ? OBTAIN : CONSUME,text_shortname);
            if(aaa->name_legal())
            {
                window->change_mod(before_mod,text_name,new Formula(text_formula),
                                   button_type == 1 ? OBTAIN : CONSUME, change_type == 1 ? 1 : 0,text_shortname);
                close();
            }
            else
            {
                show_warning("添加模板", "请检查模板内容输入。");
            }
            delete aaa;
        });
        connect(cancelButton, &QPushButton::clicked,[=](){
            close();
        });
    }

private:
    QButtonGroup buttonGroup1;
    QButtonGroup buttonGroup;
    Mod *before_mod;
};


//标签搜索
class Tags_shearch_Dialog : public QDialog {

public:
    ModWindow *window;
    Tags_shearch_Dialog(ModWindow *window, QWidget *parent = nullptr)
        : window(window), QDialog(parent) {

        QFont font("Microsoft YaHei UI", 18);//字体

        setWindowTitle(QString("选择标签"));

        QVBoxLayout *layout = new QVBoxLayout(this);
        QVBoxLayout* labelsLayout = new QVBoxLayout();
        QScrollArea* scrollArea = new QScrollArea();


        scrollArea->setStyleSheet("background-color: white;");

        // 设置滚动区域
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(new QWidget());
        scrollArea->widget()->setLayout(labelsLayout);

        //qDebug()<< window->data->totallabels.size();

        for(int i=0;i<window->data->totallabels.size();i+=4)
        {
            //qDebug()<<window->data->totallabels[i];
            QHBoxLayout *linelayout=new QHBoxLayout;
            for(int j=0;j<=3;j++)
            {
                if(i+j<window->data->totallabels.size())
                {
                    QPushButton* hh = new QPushButton(window->data->totallabels[i+j]);
                    hh->setFixedWidth(100); // 设置按钮宽度为100像素
                    if(window->ischose[i+j]==0)
                        hh->setStyleSheet("color: white; background-color: rgba(0, 0, 255, 188); border-radius: 10px");
                    else
                        hh->setStyleSheet("color: white; background-color: rgba(0, 255, 255, 188); border-radius: 10px");
                    hh->setFont(font);
                    connect(hh, &QPushButton::clicked,[=](){
                        window->ischose[i+j]=(! (window->ischose[i+j]));
                        if(window->ischose[i+j]==0)
                            hh->setStyleSheet("color: white; background-color: rgba(0, 0, 255, 188); border-radius: 10px");
                        else
                            hh->setStyleSheet("color: white; background-color: rgba(0, 255, 255, 188); border-radius: 10px");
                    });
                    linelayout->addWidget(hh);
                    linelayout->addSpacing(10);
                }
            }
            QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Maximum);
            linelayout->addItem(spacer);
            labelsLayout->addLayout(linelayout);
            labelsLayout->addSpacing(10);
        }
        QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
        labelsLayout->addItem(spacer);
        layout->addWidget(scrollArea);
        //setLayout(layout);
        setFixedSize(500, 300);

    }

private:
    QButtonGroup buttonGroup;
};



void ModWindow::setup_mods() {
    QWidget *scrollWidget = new QWidget;
    scrollWidget->setStyleSheet("background: transparent;");
    scrollWidget->setAttribute(Qt::WA_TranslucentBackground);
    QVBoxLayout *layout = new QVBoxLayout;
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->setSpacing(20);
    QFont font("Microsoft YaHei UI", 18);//字体
    QFont font1("Microsoft YaHei UI", 14);//字体


    //列出所有模板:
    for (int i=1;i<=search_count;i++) {
        Mod* x=data->mods[mod_search[i]];
        //qDebug()<<x->id<<" "<<x->print_name()<<" "<<x->get_fun()<<" "<<x->input_num<<"\n";
        //for(int j=0;j<x->input_num;j++)
        //    qDebug()<<x->variable[j]<<" ";
        if(x->is_deleted())
            continue;
        QHBoxLayout *temp=new QHBoxLayout; //居中

        //获取与消耗
        QLabel *mod_type;
        if(x->type==1)
        {
            mod_type=new QLabel("获取");
            mod_type->setStyleSheet("color: #0080FF; background-color: rgba(0, 47, 167, 50); border-radius: 12px");
        }
        else
        {
            mod_type=new QLabel("消耗");
            mod_type->setStyleSheet("color: #FF0000; background-color: rgba(255, 0, 0, 50); border-radius: 12px");
        }
        mod_type->setAlignment(Qt::AlignCenter);
        mod_type->setFont(font1);
        mod_type->setFixedWidth(80);


        //名字与公式
        QLabel *shortname_label=new QLabel(x->get_shortname());
        shortname_label->setAlignment(Qt::AlignCenter);
        shortname_label->setFixedWidth(200);
        QLabel *name_label=new QLabel(x->get_name());
        //name_label->setAlignment(Qt::AlignCenter);
        //name_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
        //name_label->setFixedWidth(300);

        //QLabel *fun_label=new QLabel(x->get_fun());
        //fun_label->setAlignment(Qt::AlignCenter);

        //删除更改按钮
        QPushButton *delete_button=create_icon_button("delete", 26,[=](){
            ConfirmDialog *confirmDialog = new ConfirmDialog(this, "删除模版", "确认删除模版吗？删除后，之前的记录不会受到影响。");
            connect(confirmDialog, &ConfirmDialog::confirmed, [=] () {
                delete_mod(x);
                setup_mods();
            });
            confirmDialog->show();
        });
        QPushButton *changeButton = create_icon_button("edit",24,[=]{
            ChangeDialog dialog(this, x);
            dialog.exec();
            setup_mods();
        });

        TagButton *tagbutton = new TagButton(this, x);
        tagbutton->setIcon(QIcon(":/images/tag"));
        tagbutton->setIconSize(QSize(24, 24));
        tagbutton->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
        tagbutton->setStyleSheet("QPushButton { border: none; outline: none; }");

        //设置字体
        shortname_label->setFont(font);
        name_label->setFont(font);
        //fun_label->setFont(font);

        // 创建一个左边留白的QSpacerItem
        QSpacerItem *leftSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
        temp->addItem(leftSpacer);

        temp->addWidget(mod_type);
        temp->addWidget(shortname_label);
        temp->addWidget(name_label);
        //temp->addWidget(fun_label);
        temp->addWidget(changeButton);
        temp->addWidget(delete_button);
        temp->addWidget(tagbutton);


        // 创建一个右边留白的QSpacerItem
        QSpacerItem *rightSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
        temp->addItem(rightSpacer);

        QString strDes = QFontMetrics(name_label->font()).elidedText(x->get_name(), Qt::ElideRight, name_label->width());
        name_label->setText(strDes);

        layout->addLayout(temp);
    }
    layout->addItem(spacer);

    scrollWidget->setLayout(layout);
    QScrollArea *scrollArea = ui->scrollArea;
    scrollArea->setWidget(scrollWidget);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent;");
    scrollArea->setAttribute(Qt::WA_TranslucentBackground);
}

void ModWindow::init_mods() {
    mod_cnt=data->mods.size();
    for(int i=0;i<data->mods.size();i++)
    {
        mod_search[++search_count]=i;
    }
    for (Mod *mod : data->mods) {
        if(mod->is_deleted())
            continue;
        for (QString &label : mod->labels) {
            if (std::find(data->totallabels.begin(), data->totallabels.end(), label) == data->totallabels.end()) {
                data->totallabels.push_back(label);
            }
        }
    }
    memset(ischose,0, sizeof(ischose));
    std::vector<QString> b;
    search("",b);
}

ModWindow::ModWindow(Data *data, QWidget *parent)
    : data(data), QMainWindow(parent), ui(new Ui::ModWindow)
{
    QFont font("Microsoft YaHei UI", 14);//字体

    ui->setupUi(this);

    //添加模板
    QPushButton *addButton = create_icon_button("plus",40,[=]{
        AddDialog dialog(this);
        dialog.exec();
        setup_mods();
    });


    QLineEdit *lineEdit_search_string = new QLineEdit();
    lineEdit_search_string->setFont(font);


    lineEdit_search_string->setPlaceholderText("请输入搜索内容、选择标签后点击搜索按钮");

    QString textEditStyle = "QLineEdit {background: transparent; border: 1px solid gray; border-radius: 17px; "
                            "padding-left: 10px; padding-right: 10px; padding-top: 5px; padding-bottom: 5px; }"
                            "QLineEdit:focus { border-color: #add8e6; }";
    lineEdit_search_string->setStyleSheet(textEditStyle);

    QPushButton *tagsButton = create_icon_button("tag",32,[=]{
        Tags_shearch_Dialog dialog(this);
        dialog.exec();
    });

    QVBoxLayout *picture1=new QVBoxLayout();
    picture1->addSpacing(4);
    picture1->addWidget(tagsButton);
    picture1->addSpacing(4);

    //搜索按钮
    QPushButton *searchButton = create_icon_button("search",36,[=]{
        std::vector<QString> b;
        for(int i=0;i<data->totallabels.size();i++)
            if(ischose[i])
                b.push_back(data->totallabels[i]);
        search(lineEdit_search_string->text(),b);
        setup_mods();
    });

    // 回车搜索
    connect(lineEdit_search_string, &QLineEdit::returnPressed, this, [=](){
        std::vector<QString> b;
        for(int i=0;i<data->totallabels.size();i++)
            if(ischose[i])
                b.push_back(data->totallabels[i]);
        search(lineEdit_search_string->text(),b);
        setup_mods();
    });

    QVBoxLayout *picture2=new QVBoxLayout();
    picture2->addSpacing(3);
    picture2->addWidget(searchButton);
    picture2->addSpacing(2);


    ui->horizontalLayout_2->addWidget(lineEdit_search_string);
    ui->horizontalLayout_2->addSpacing(5);
    ui->horizontalLayout_2->addLayout(picture1);
    ui->horizontalLayout_2->addSpacing(5);
    ui->horizontalLayout_2->addLayout(picture2);
    ui->horizontalLayout_2->addWidget(addButton);

    // todo: 读入的 mods 中包含标签时，搜索标签功能失效

    setup_mods();

    connect(data, &Data::mod_added, this, &ModWindow::on_mod_added);
    connect(data, &Data::mod_modified, this, &ModWindow::on_mod_modified);
    connect(data, &Data::all_mod_changed, this, &ModWindow::on_all_mod_changed);
}


ModWindow::~ModWindow()
{
    delete ui;
}


void ModWindow::add_mod(QString name, Formula *formula, enum RECORD_TYPE type, QString short_name){
    Mod* mod = new Mod(mod_cnt, name, formula, type, short_name);// id从0开始
    mod->create_uuid();
    int id = db_add_mod(data, mod);
    mod->set_id(id);
    data->mods.push_back(mod);
    mod_cnt++;
    mod_search[++search_count]=mod_cnt-1;
}

void ModWindow::delete_mod(Mod *mod){
    mod->set_deleted(true);
    for(int i=0;i<mod->labels.size();i++)
        delete_label(mod, mod->labels[i]);
    db_modify_mod(data, mod);
}

// change_legacy: 是否更新之前这一模板留下的记录
void ModWindow::change_mod(Mod *before_mod, QString name, Formula *formula, enum RECORD_TYPE type, int change_legacy, QString short_name){
    // 0则为不更改，1为更改
    if(change_legacy==0)
    {
        Mod *mod = new Mod(mod_cnt, name, formula, type, short_name);
        mod_cnt++;
        data->mods.push_back(mod);
        for(int i=0;i<(before_mod->labels).size();i++)
            add_label(mod, before_mod->labels[i]);
        delete_mod(before_mod);
        mod_search[++search_count]=mod_cnt-1;

        db_modify_mod(data, before_mod);
        mod->create_uuid();
        mod->set_id(db_add_mod(data, mod));
    }
    else
    {
        before_mod->change(name,formula,type,short_name);
        db_modify_mod(data, before_mod);
        /////////////////////////////  还要更改总积分，要看记录怎么写   /////////////////////////////

    }
}

void ModWindow::search_string(QString aa){//字符串搜索
    temp_count=0;
    for(int i=1;i<=search_count;i++)
    {
        if(data->mods[mod_search[i]]->is_deleted()==true)//如果是被删除的模板就跳过
            continue;
        if(data->mods[mod_search[i]]->search_str(aa)==true)
        {
            temp_search[++temp_count]=mod_search[i];//如果找到一个符合要求的模板就加进temp里
        }
    }
    search_count=temp_count;
    for(int i=1;i<=temp_count;i++)
        mod_search[i]=temp_search[i];
}

void ModWindow::search_tag(QString aa){
    temp_count=0;
    for(int i=1;i<=search_count;i++)
    {
        if(data->mods[mod_search[i]]->is_deleted()==true)//如果是被删除的模板就跳过
            continue;
        if(data->mods[mod_search[i]]->find_label(aa)==true)
        {
            temp_search[++temp_count]=mod_search[i];//如果找到一个符合要求的模板就加进temp里
        }
    }
    search_count=temp_count;
    for(int i=1;i<=temp_count;i++)
        mod_search[i]=temp_search[i];
}

void ModWindow::search(QString a, std::vector<QString> b){ //a是查询的字符串，b是标签的数组
    search_count=data->mods.size();
    for(int i=1;i<=search_count;i++)
        mod_search[i]=i-1;
    search_string(a);
    for(int i=0;i<b.size();i++)
        search_tag(b[i]);
    if(search_count==0)
        qDebug() << "未搜索到结果";
    else
    {
        for(int i=1;i<=search_count;i++)
        {
            data->mods[mod_search[i]]->get_name();
            /////////////////////////////  要结合ui   /////////////////////////////
        }
    }
}

void ModWindow::add_label(Mod *mod, QString label){ //加标签
    if(std::find(data->totallabels.begin(), data->totallabels.end(), label)==data->totallabels.end())
        data->totallabels.push_back(label);
    mod->labels.push_back(label);
    db_modify_mod(data, mod);
    emit data->label_modified(mod);
}

void ModWindow::delete_label(Mod *mod, QString label){ //删除标签
    auto it=std::find(mod->labels.begin(), mod->labels.end(), label);
    if(it==mod->labels.end())
        qDebug() << "未找到改标签";
    else
    {
        mod->labels.erase(it);
    }
    int flagg=0;
    for(int i=0;i<data->mods.size();i++)
    {
        if(data->mods[i]->is_deleted())
            continue;
        if(std::find( (data->mods[i]->labels).begin() , (data->mods[i]->labels).end() , label ) != (data->mods[i]->labels).end() )
        {
            flagg=1;
            break;
        }
    }
    auto itt=std::find(data->totallabels.begin(), data->totallabels.end(), label);
    if(flagg==0)
    {
        data->totallabels.erase(itt);
        memset(ischose,0,sizeof(ischose));
    }
    db_modify_mod(data, mod);
    emit data->label_modified(mod);
}

void ModWindow::on_mod_added(Mod *mod) {
    mod_cnt++;
    mod_search[++search_count]=mod_cnt-1;
    setup_mods();
}

void ModWindow::on_mod_modified(Mod *mod) {
    setup_mods();
}

void ModWindow::on_all_mod_changed() {
    init_mods();
    setup_mods();
}
