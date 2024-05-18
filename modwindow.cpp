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

class TagButton : public QPushButton {

public:
    TagButton(QWidget *parent = nullptr) : QPushButton(parent) {
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
    void showDialog() {
        // 创建对话框并设置布局
        dialog = new QDialog(this);
        //dialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
        QVBoxLayout *layout = new QVBoxLayout(dialog);

        // 添加标签到对话框中
        QLabel *label1 = new QLabel("标签1", dialog);
        QLabel *label2 = new QLabel("标签2", dialog);
        layout->addWidget(label1);
        layout->addWidget(label2);

        // 设置对话框位置
        dialog->setFixedSize(200, 100);
        //dialog->move(pos().x(),pos().y()+size().height()+80);
        QPoint newPos = this->mapToGlobal(QPoint(0, this->size().height()));
        dialog->move(newPos.x()-100, newPos.y() + 10);

        // 显示对话框
        dialog->show();

        // 安装对话框的事件过滤器
        dialog->installEventFilter(this);
    }

    void hideDialogAfterDelay() {
        hideTimer.start(1500);
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


class Warning_Dialog : public QDialog {

public:
    Warning_Dialog(QWidget *parent = nullptr) : QDialog(parent) {

        QFont font("consolos", 12);//字体


        setWindowTitle(QString("添加模板"));
        setFixedSize(200, 100);


        // 创建文本输入框
        QLabel *label_name=new QLabel("请检查模板内容输入");
        label_name->setFont(font);
        //ok，cancel按钮
        QPushButton *okButton = new QPushButton("确定");

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(label_name);
        layout->addWidget(okButton);
        setLayout(layout);

        // 连接确定按钮的点击事件
        connect(okButton, &QPushButton::clicked, [=]() {
            close();
        });
    }
};


//添加对话框
class AddDialog : public QDialog {

public:
    AddDialog(QWidget *parent = nullptr) : QDialog(parent) {

        QFont font("consolos", 12);//字体


        setWindowTitle(QString("添加模板"));
        setFixedSize(500, 300);

        // 创建Radio button组
        QRadioButton *consumeButton = new QRadioButton("花费");
        consumeButton->setFont(font);
        QRadioButton *obtainButton = new QRadioButton("获得");
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

        //ok，cancel按钮
        QPushButton *okButton = new QPushButton("确定");
        QPushButton *cancelButton = new QPushButton("取消");
        QHBoxLayout *ok_cancel_button=new QHBoxLayout;
        ok_cancel_button->addWidget(cancelButton);
        ok_cancel_button->addItem(new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum));
        ok_cancel_button->addWidget(okButton);



        QVBoxLayout *layout = new QVBoxLayout;
        layout->addLayout(buttonlayout);
        layout->addLayout(input_shortname);
        layout->addLayout(input_name);
        layout->addLayout(input_formula);
        layout->addLayout(ok_cancel_button);
        setLayout(layout);

        // 连接确定按钮的点击事件
        connect(okButton, &QPushButton::clicked, [=]() {
            int button_type=(obtainButton->isChecked()) ? 1 : 2;
            QString text_name = lineEdit_name->text();
            QString text_shortname = lineEdit_shortname->text();
            QString text_formula = lineEdit_formula->text();

            Formula bb(text_formula);
            //qDebug()<<"here";
            Mod* aaa=new Mod(0,text_name,&bb, button_type == 1 ? OBTAIN : CONSUME , text_shortname);
            //qDebug()<<"here1";
            if(aaa->name_legal())
            {
                Mod::add_mod(text_name,new Formula(text_formula), button_type == 1 ? OBTAIN : CONSUME, text_shortname);
                close();
            }
            else
            {
                Warning_Dialog dialog;
                dialog.exec();
            }
            delete aaa;
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
    ChangeDialog(int id,QWidget *parent = nullptr) : before_id(id), QDialog(parent) {

        QFont font("consolos", 12);//字体


        setWindowTitle(QString("添加模板"));
        setFixedSize(500, 300);


        // 创建Radio button组
        QRadioButton *yeschangeButton = new QRadioButton("是");
        yeschangeButton->setChecked(true);
        QRadioButton *nochangeButton = new QRadioButton("否");
        QLabel *label1=new QLabel("是否更新之前的记录");
        label1->setFont(font);

        // 将Radio button添加到按钮组
        buttonGroup1.addButton(yeschangeButton);
        buttonGroup1.addButton(nochangeButton);

        QHBoxLayout *buttonlayout1=new QHBoxLayout;
        buttonlayout1->addWidget(label1);
        buttonlayout1->addWidget(yeschangeButton);
        buttonlayout1->addWidget(nochangeButton);



        // 创建Radio button组
        QRadioButton *consumeButton = new QRadioButton("花费");
        consumeButton->setFont(font);
        QRadioButton *obtainButton = new QRadioButton("获得");
        obtainButton->setFont(font);
        if(mods[id]->type==1)
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
        QLineEdit *lineEdit_shortname = new QLineEdit();
        lineEdit_shortname->setText(mods[id]->get_shortname());
        lineEdit_shortname->setFont(font);
        QLineEdit *lineEdit_name = new QLineEdit();
        lineEdit_name->setText(mods[id]->get_name());
        lineEdit_name->setFont(font);
        QLineEdit *lineEdit_formula=new QLineEdit;
        lineEdit_formula->setText(mods[id]->get_fun());
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

        //ok，cancel按钮
        QPushButton *okButton = new QPushButton("确定");
        QPushButton *cancelButton = new QPushButton("取消");
        QHBoxLayout *ok_cancel_button=new QHBoxLayout;
        ok_cancel_button->addWidget(cancelButton);
        ok_cancel_button->addItem(new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum));
        ok_cancel_button->addWidget(okButton);



        QVBoxLayout *layout = new QVBoxLayout;
        layout->addLayout(buttonlayout1);
        layout->addLayout(buttonlayout);
        layout->addLayout(input_shortname);
        layout->addLayout(input_name);
        layout->addLayout(input_formula);
        layout->addLayout(ok_cancel_button);
        setLayout(layout);

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
                Mod::change_mod(before_id,text_name,new Formula(text_formula),
                                button_type == 1 ? OBTAIN : CONSUME, change_type == 1 ? OBTAIN : CONSUME,text_shortname);
                close();
            }
            else
            {
                Warning_Dialog dialog;
                dialog.exec();
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
    int before_id;
};


void setup_mods(Ui::ModWindow *ui) {
    QWidget *scrollWidget = new QWidget;
    scrollWidget->setStyleSheet("background-color: white;");
    QVBoxLayout *layout = new QVBoxLayout;
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->setSpacing(20);
    QFont font("consolos", 18);//字体
    QFont font1("consolos", 14);//字体


    //列出所有模板:
    for (int i=1;i<=Mod::search_count;i++) {
        Mod* x=mods[Mod::mod_search[i]];
        //qDebug()<<x->id<<" "<<x->print_name()<<" "<<x->get_fun()<<" "<<x->input_num<<"\n";
        //for(int j=0;j<x->input_num;j++)
        //    qDebug()<<x->variable[j]<<" ";
        if(x->is_deleted())
            continue;
        QHBoxLayout *temp=new QHBoxLayout; //居中

        //获得与花费
        QLabel *mod_type;
        if(x->type==1)
        {
            mod_type=new QLabel("获得");
            mod_type->setStyleSheet("color: black; background-color: rgba(0, 255, 0, 188); border-radius: 10px");
        }
        else
        {
            mod_type=new QLabel("花费");
            mod_type->setStyleSheet("color: white; background-color: rgba(255, 0, 0, 188); border-radius: 10px");
        }
        mod_type->setAlignment(Qt::AlignCenter);
        mod_type->setFont(font1);
        mod_type->setFixedWidth(80);


        //名字与公式
        QLabel *shortname_label=new QLabel(x->get_shortname());
        shortname_label->setAlignment(Qt::AlignCenter);
        shortname_label->setFixedWidth(200);
        QLabel *name_label=new QLabel(x->get_name());
        name_label->setAlignment(Qt::AlignCenter);
        //QLabel *fun_label=new QLabel(x->get_fun());
        //fun_label->setAlignment(Qt::AlignCenter);

        //删除更改按钮
        QPushButton *delete_button=create_icon_button("delete", 24,[=](){
            x->delete_mod();
            setup_mods(ui);
        });
        QPushButton *changeButton = create_icon_button("edit",24,[=]{
            ChangeDialog dialog(x->id);
            dialog.exec();
            setup_mods(ui);
        });

        TagButton *tagbutton = new TagButton;
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

        layout->addLayout(temp);
    }
    layout->addItem(spacer);

    scrollWidget->setLayout(layout);
    QScrollArea *scrollArea = ui->scrollArea;
    scrollArea->setWidget(scrollWidget);  // 将QWidget设置为滚动区域的内部窗口
    scrollArea->setWidgetResizable(true); //取消边界框
}


ModWindow::ModWindow(Data *data, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ModWindow)
{
    Mod::add_mod(QString("学习数学分析{x}分钟"),new Formula(QString("x^2")), OBTAIN,"学习高代");
    Mod::add_mod(QString("学习高等代数{x}分钟"),new Formula(QString("2*x")), OBTAIN,"学习数分");
    Mod::add_mod(QString("玩第五人格{x}分钟"),new Formula(QString("x+1")), CONSUME,"第五人格");
    Mod::add_mod(QString("玩原神{x}分钟"),new Formula(QString("x/2")), CONSUME,"原神");
    Mod::add_mod(QString("跑步{x}分钟，速度{y}"),new Formula(QString("10*x/y")), OBTAIN,"跑步锻炼");

    ui->setupUi(this);

    //添加模板
    QPushButton *addButton = create_icon_button("plus",34,[=]{
        AddDialog dialog;
        dialog.exec();
        setup_mods(ui);
    });
    QLineEdit *lineEdit_search_string = new QLineEdit();
    QPushButton *searchButton = create_icon_button("search",34,[=]{
        std::vector<QString> b;
        Mod::search(lineEdit_search_string->text(),b);
        setup_mods(ui);
    });
    ui->horizontalLayout_2->addWidget(lineEdit_search_string);
    ui->horizontalLayout_2->addWidget(searchButton);
    ui->horizontalLayout_2->addWidget(addButton);
    std::vector<QString> b;
    Mod::search("",b);

    setup_mods(ui);

}


ModWindow::~ModWindow()
{
    delete ui;
}
