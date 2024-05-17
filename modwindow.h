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

private:
    Ui::ModWindow *ui;
    Data *data;
};
#endif // MODWINDOW_H
