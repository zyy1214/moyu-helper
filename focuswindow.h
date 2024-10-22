#ifndef FOCUSWINDOW_H
#define FOCUSWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui {
class FocusWindow;
}
QT_END_NAMESPACE

class FocusWindow : public QMainWindow
{
    Q_OBJECT

public:
    FocusWindow(bool flag, int time = 0 , QWidget *parent = nullptr);
    ~FocusWindow();
    bool is_night=0;
protected:
    void closeEvent(QCloseEvent *event) override; // 声明 closeEvent 方法
    void paintEvent(QPaintEvent *event) override;
    void checkAndBringToFront() ;
private:
    bool flag;
    bool canclose;
    int countdowntime;
    Ui::FocusWindow *ui;
    QDateTime startTime; // 声明 QDateTime 对象，用于记录窗口打开时的时间
    int picturetype=0;
    QMainWindow a;
private slots:
    void updateTime(); // 声明更新时间的槽函数
};
#endif // FOCUSWINDOW_H
