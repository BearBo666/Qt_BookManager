#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/res/Book.png"));
    ui->Tip->hide();

    //用户名和密码的实现
    this->user="admin";
    this->password="123456";

    //登录功能的实现
    connect(ui->pushButton,&QPushButton::clicked,[=](){
        //判断用户名和密码是否正确
        if(ui->usernameEdit->text()==user&&ui->passwordEdit->text()==password){
            ui->Tip->hide();
            this->hide();
            mainWindow=new MainWindow;
            mainWindow->show();
        }else {
            ui->Tip->show();
        }
    });

    //退出功能的实现
    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        this->close();
    });
}

Widget::~Widget()
{
    delete ui;
}
