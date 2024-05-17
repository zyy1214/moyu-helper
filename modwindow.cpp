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

#include "data_storage.h"


class Warning_Dialog : public QDialog {

public:
    Warning_Dialog(QWidget *parent = nullptr) : QDialog(parent) {

        QFont font("consolos", 12);//字体


        setWindowTitle(QString("添加模板"));
        setFixedSize(200, 100);


        // 创建文本输入框
        QLabel *label_name=new QLabel("请检查模板名字输入");
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
        QLabel *label_name=new QLabel("请输入模板名称：");
        label_name->setFont(font);
        QLabel *label_formula=new QLabel("请输入公式：");
        label_formula->setFont(font);
        QLineEdit *lineEdit_name = new QLineEdit();
        lineEdit_name->setFont(font);
        QLineEdit *lineEdit_formula=new QLineEdit;
        lineEdit_formula->setFont(font);
        QHBoxLayout *input_name=new QHBoxLayout;
        QHBoxLayout *input_formula=new QHBoxLayout;
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
        layout->addLayout(input_name);
        layout->addLayout(input_formula);
        layout->addLayout(ok_cancel_button);
        setLayout(layout);

        // 连接确定按钮的点击事件
        connect(okButton, &QPushButton::clicked, [=]() {
            int button_type=(obtainButton->isChecked()) ? 1 : 2;
            QString text_name = lineEdit_name->text();
            QString text_formula = lineEdit_formula->text();

            Formula bb(text_formula);
            //qDebug()<<"here";
            Mod* aaa=new Mod(0,text_name,&bb, button_type == 1 ? OBTAIN : CONSUME);
            //qDebug()<<"here1";
            if(aaa->name_legal())
            {
                Mod::add_mod(text_name,new Formula(text_formula), button_type == 1 ? OBTAIN : CONSUME);
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
        QLabel *label_name=new QLabel("请输入模板名称：");
        label_name->setFont(font);
        QLabel *label_formula=new QLabel("请输入公式：");
        label_formula->setFont(font);
        QLineEdit *lineEdit_name = new QLineEdit();
        lineEdit_name->setText(mods[id]->print_name());
        lineEdit_name->setFont(font);
        QLineEdit *lineEdit_formula=new QLineEdit;
        lineEdit_formula->setText(mods[id]->get_fun());
        lineEdit_formula->setFont(font);
        QHBoxLayout *input_name=new QHBoxLayout;
        QHBoxLayout *input_formula=new QHBoxLayout;
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

            Formula bb(text_formula);
            Mod* aaa=new Mod(0,text_name,&bb, button_type == 1 ? OBTAIN : CONSUME);
            if(aaa->name_legal())
            {
                Mod::change_mod(before_id,text_name,new Formula(text_formula),
                                button_type == 1 ? OBTAIN : CONSUME, change_type == 1 ? OBTAIN : CONSUME);
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
    QFont font("consolos", 12);//字体


    //列出所有模板:
    for (int i=1;i<=Mod::search_count;i++) {
        Mod* x=mods[Mod::mod_search[i]];
        //qDebug()<<x->id<<" "<<x->print_name()<<" "<<x->get_fun()<<" "<<x->input_num<<"\n";
        //for(int j=0;j<x->input_num;j++)
        //    qDebug()<<x->variable[j]<<" ";
        if(x->is_deleted())
            continue;
        QHBoxLayout *temp=new QHBoxLayout;

        //获得与花费
        QLabel *mod_type;
        if(x->type==1)
        {
            mod_type=new QLabel("获得");
            mod_type->setStyleSheet("color: white; background-color: green");
        }
        else
        {
            mod_type=new QLabel("花费");
            mod_type->setStyleSheet("color: white; background-color: red");
        }
        mod_type->setAlignment(Qt::AlignCenter);
        mod_type->setFont(font);
        mod_type->setFixedWidth(100);


        //名字与公式
        QLabel *name_label=new QLabel(x->print_name());
        QLabel *fun_label=new QLabel(x->get_fun());
        QPushButton *delete_button=create_icon_button("delete", 24,[=](){
            x->delete_mod();
            setup_mods(ui);
        });
        QPushButton *changeButton = create_icon_button("edit",24,[=]{
            ChangeDialog dialog(x->id);
            dialog.exec();
            setup_mods(ui);
        });

        //设置字体
        name_label->setFont(font);
        fun_label->setFont(font);

        temp->addWidget(mod_type);
        temp->addWidget(name_label);
        temp->addWidget(fun_label);
        temp->addWidget(changeButton);
        temp->addWidget(delete_button);
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
    Mod::add_mod(QString("学习数学分析{x}分钟"),new Formula(QString("x^2")), OBTAIN);
    Mod::add_mod(QString("学习高等代数{x}分钟"),new Formula(QString("2*x")), OBTAIN);
    Mod::add_mod(QString("学习数学分析{x}分钟"),new Formula(QString("x+1")), CONSUME);
    Mod::add_mod(QString("学习高等代数{x}分钟"),new Formula(QString("x/2")), CONSUME);
    Mod::add_mod(QString("学习数学分析{x}分钟"),new Formula(QString("x+3")), OBTAIN);
    Mod::add_mod(QString("学习高等代数{x}分钟"),new Formula(QString("3*x-2")), CONSUME);

    ui->setupUi(this);

    //添加模板
    QPushButton *addButton = create_icon_button("plus",24,[=]{
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
