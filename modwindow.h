#ifndef MODWINDOW_H
#define MODWINDOW_H

#include <QMainWindow>

#include "data_storage.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ModWindow;
}
QT_END_NAMESPACE

class ModWindow : public QMainWindow
{
    Q_OBJECT

public:
    ModWindow(Data *data, QWidget *parent = nullptr);
    ~ModWindow();
    Data *data;

    void add_mod(QString a, Formula *b, enum RECORD_TYPE type,QString shortn);
    void delete_mod(Mod *mod);//删除模板
    void change_mod(Mod *before_mod, QString a, Formula *b, enum RECORD_TYPE type, int change_type, QString shortn); //更改模板，先加再删
    void search_string(QString aa); //字符串搜索
    void search_tag(QString aa);
    void search(QString a, std::vector<QString> b);//a是查询的字符串，b是标签的数组

    void add_label(Mod *mod, QString label); //加标签
    void delete_label(Mod *mod, QString label); //删除标签

    int search_count=0;
    int temp_count=0;
    int mod_cnt=0;
    int temp_search[500] = {};
    int mod_search[500]={};
    bool ischose[2000]; //是否选择这个标签

private:
    Ui::ModWindow *ui;

    void setup_mods();

private slots:
    void on_mod_added(Mod *mod);
    void on_mod_modified(Mod *mod);
};
#endif // MODWINDOW_H
