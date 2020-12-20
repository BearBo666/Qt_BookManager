#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include"mainwindow.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    //登录用的用户名和密码
    QString user;
    QString password;

    //主窗体指针
    MainWindow * mainWindow=nullptr;
private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
